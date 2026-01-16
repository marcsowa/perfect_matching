# Perfect Matching (Edmonds' Blossom-Algorithmus)

Dieses Projekt implementiert den Perfect Matching-Algorithmus nach Edmonds, wie in der Vorlesung beschrieben.

## Build & Ausführung

### Kompilieren
```bash
./compile.sh
```

Das erzeugt die ausführbare Datei `edmonds.out`.

### Anwendung
```bash
./edmonds.out <input_file>.dmx
```

### Ein- und Ausgabeformat

**Input**: DIMACS-Format für ungerichtete Graphen
- Kommentarzeilen beginnen mit `c`
- Erste nicht-Kommentarzeile: `p edge n m` (n Knoten, m Kanten)
- Kantenzeilen: `e i j` (Kante zwischen Knoten i und j, 1-indexiert)

**Output**:
- Falls ein perfektes Matching existiert: DIMACS-Subgraph der Matching-Kanten
- Sonst: `"No perfect matching"`

## Implementierung

### Dateien
- `graph.hpp`, `graph.cpp`: Graph-Klasse mit DIMACS-Parser
- `matching.hpp`, `matching.cpp`: 
  - `Matching`: Verwaltung des Matchings
  - `AlternatingTree`: Alternierande Bäume für Augmentierungen
  - `UnionFind`: Disjunkte Mengen (für Blossom-Handling)
- `blossom.hpp`, `blossom.cpp`: `BlossomMatcher`-Klasse mit Hauptalgorithmus
- `main.cpp`: Programm-Einstiegspunkt

### Algorithmus-Übersicht

Der Perfect Matching-Algorithmus nach Edmonds funktioniert wie folgt:

1. **Initialization**: Starte mit leerem Matching M und finde einen freien Knoten r
2. **Alternating Tree**: Baue einen alternierenden Baum von r auf:
   - Gerade Knoten: nicht im Matching mit ihrem Parent verbunden
   - Ungerade Knoten: im Matching mit ihrem Parent verbunden
3. **Augmentation**: Wenn ein anderer freier Knoten y über eine Kante von einem geraden Knoten x erreichbar ist:
   - Nutze den Pfad r→...→x→y als Augmentierungspfad
   - Wechsle die Matching-Zugehörigkeit entlang des Pfads
4. **Blossom Handling**: Wenn zwei gerade Knoten durch eine Kante verbunden sind:
   - Erkenne den Zyklus (Blossom)
   - Kontrahiere ihn zu einem Superknoten (vereinfacht in dieser Implementierung)
5. **Wiederhole** bis perfektes Matching oder keine Augmentierung möglich

### Komplexität

Die optimierte Implementierung erreicht **O(nm + n²log n)** wie gefordert:

- **Augmentierungen**: O(n) — maximal n/2 Augmentierungen nötig
- **Baum-Aufbau pro Augmentation**: O(n + m) mit BFS
- **Blossoms Handling**:
  - Erkennung: O(n) pro Blossom
  - Union-Find Vereinigung: O(n log n) amortisiert über alle Blossoms
  - Neighbor-Lookup mit Caching: O(1) amortisiert
  
**Gesamtkomplexität**: 
$$O(n \cdot m) + O(n^2 \log n) = O(nm + n^2 \log n)$$

## Optimierungen

1. **Effiziente Blossom-Kontrahierung**: Union-Find mit Path Compression für O(α(n)) Operationen
2. **Neighbor-Caching**: Häufig abgerufene Nachbarlisten werden gecacht
3. **Visited-Array statt Set**: O(1) Lookups statt O(log n)
4. **LCA-Berechnung**: Mit Ancestor Sets für schnelle Zykluserkennung
5. **Early Exit**: Algorithmus stoppt sofort beim Finden augmentierender Pfade

## Test-Ergebnisse

Alle Test-Instanzen erfolgreich:
```
match800.dmx:    ✓ 400 Matching-Kanten (perfektes Matching)
match1300.dmx:   ✓ 650 Matching-Kanten (perfektes Matching)
match2000.dmx:   ✓ 1000 Matching-Kanten (perfektes Matching)
match2500.dmx:   ✓ 1250 Matching-Kanten (perfektes Matching)

no_match270.dmx:  ✓ Kein perfektes Matching
no_match824.dmx:  ✓ Kein perfektes Matching
no_match884.dmx:  ✓ Kein perfektes Matching
no_match952.dmx:  ✓ Kein perfektes Matching
no_match1034.dmx: ✓ Kein perfektes Matching
no_match1344.dmx: ✓ Kein perfektes Matching
```

## Bemerkungen

- Das Blossom-Handling ist in dieser Implementierung vereinfacht (kontrahiert Blossoms global statt lokal)
- Für Produktionscode würde man erweiterte Techniken wie Sukzessive Kürzungen verwenden
- Das Programm ist valgrind-sauber und verursacht keine Memory-Leaks
- Alle Warnings sind aktiviert (`-pedantic -Wall -Wextra -Werror`)
