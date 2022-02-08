# HB-UNI-Sen-PM

![HB-UNI-Sen-PM](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/komplett_seite.JPG)

Eine AskSinPP-Implementierung eines Feinstaubsensors fuer den Aussenbereich mit dem PM-Sensor SPS30 sowie dem Feuchtesensor SHT31 von Sensirion. Das Geraet kann regulaer in eine Homematic-Umgebung eingebunden werden und liefert folgende Messwerte:

- Feinstaubkonzentrationen PM1, PM2.5, PM4 und PM10 im µg/m³
- Partikelanzahlen je cm³ fuer Partikelgroessen <0.5µm, <1µm, <2.5µm, <4µm und <10µm
- Temperatur
- relative Luftfeuchte


## Platinen

Schaltplaene, Bestueckungsplaene, Gerber Files sowie Eagle Files finden sich [hier](https://github.com/HMSteve/HB-UNI-Sen-PM/tree/main/PCB).

Hauptplatine

![PCB Main](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/pcb_main.jpg)

Hauptplatine mit SHT31-Platine

![PCB Both](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/pcb_both.jpg)

Beide Platinen mit SPS30

![PCB and SPS](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/pcb_and_sps.jpg)


### Bestueckung

Die Bestueckung der Hauptplatine ist unkritisch und kann per Hand erfolgen, es werden Bauformen 0805 oder groesser genutzt.

Bei der Feuchtesensorplatine ist der SHT31 zuerst zu bestuecken. Grundsaetzlich ist Handloetung unter guter Lupe oder Mikroskop moeglich, vorteilhaft ist jedoch der Auftrag von Lotpaste mittels Schablone und Loetung mit Hitze, bspw. Heissluft, von unten durch die Platine. Bei diesem Verfahren ist keine 100% exakte Positionierung des Bauteils noetig, da es bei korrekt dosierter Lotpaste nach deren Aufschmelzen auf Grund der Oberflaechenspannung auf dem flüssigen Lot von selbst in die korrekte Position schwimmt. Lupe oder Mikroskop zur Kontrolle der Loetstellen von der Seite sollte man nutzen. Anschliessend wird die Schutzkappe in die vorgesehenen Loecher eingesteckt und haelt ohne Klebstoffe etc.
Die beiden Platinen sollten erst ganz zum Schluss, nach Test und ggf. nach Waschen der Hauptplatine, zusammengesteckt und verloetet werden. Der SHT31 darf weder Waschfluessigkeit wie Isopropanol noch deren Daempfen ausgesetzt werden. Anderenfalls ist mit einer (ohne langwieriges Backen) irreversiblen Fehlmessung der Luftfeuchte zu rechnen.

Der Loetjumper ist gemaess Datenblatt des SHT31 unbedingt nach Masse zu schliessen, um unerwuenschte Effekte wie Aussetzer bei hoher Luftfeuchte zu vermeiden. 

ACHTUNG: Version 2 der Hauptplatine besitzt einen fehlerhaften Aufdruck der Betriebsspannungspolaritaet. Dies unbedingt beachten. +5V sind an den Pin zum Platinenrand anzuschliessen, Masse an den Pin zur Platinenmitte. Auch nur kurze Verpolung kann bereits zur Zerstoerung des Elkos vor dem LDO und ggf. weiterer Bauteile fuehren.


## Software

### Addon zur Geraeteunterstuetzung

Sofern noch nicht vorhanden oder nicht aktuell, ist das [Addon](https://github.com/HMSteve/SG-HB-Devices-Addon/raw/master/CCU_RM/sg-hb-devices-addon.tgz) auf der CCU zu installieren. Der Sensor benoetigt mindestens Version 1.4.

### Firmware

Es wird ein ATMega328p mit Quarz verwendet. Deshalb kann bei Nutzung der Arduino IDE der Arduino Pro Mini ausgewaehlt und der Standard-Bootlader geflashed werden.

Um die Firmware zu kompilieren, muessen sich die .ino sowie die .h Dateien im gleichen Verzeichnis befinden, das ./sensors-Verzeichnis darunter. Zudem muss eine Reihe von Bibliotheken ueber den Library Manager eingebunden werden:
- AskSinPP
- EnableInterrupt
- LowPower
- SparkFun SCD30 Arduino Library
- Sensirion-sps30
-

Anschliessend sollte die Firmware problemlos kompilierbar und das Device nach dem Flashen anlernbar sein. Da der Anlerntaster zwecks Wetterschutz nicht von ausserhalb des Gehaeuses zugaenglich ist, muss das Anlernen vor dem Zusammenbau erfolgen.


## Gehaeuse

Das auessere Gehaeuse orientiert sich an einem aehnlichen [Projekt](https://www.thingiverse.com/thing:2821592), verwendet jedoch nur die mittleren Ringe des Stevenson Screen. Die uebrigen Dateien wurden von mir dazu passend erstellt. Alle STLs finden sich [hier](https://github.com/HMSteve/HB-UNI-Sen-PM/tree/main/3D_Druck).
Neben den 3D-Druckteilen wird folgendes Material benoetigt:

- je vier Einschmelzmuttern M3 und M8 fuer den Deckel
- ein O-Ring 5mm x 1, moeglich ist auch 6mm x 1.5 oder aehnlich, zum Abdichten der Sensirion-SF2-Filterkappe ueber dem SHT31 gegen das innere Gehaeuse
- vier Schrauben M3 x 10, Edelstahl oder Messing, zum Montieren des inneren Gehaeuses am Deckel
- EPDM-Dichtschnur 3mm zum Abdichten des inneren Gehaeuses gegen den Deckel
- vier Gewindestange M8 zzgl. vier Muttern M8 zum Zusammenbau von Deckel, Zwischenringen und Haltewinkel, Edelstahl oder Messing
- eine PG-Verschraubung M12 zur Einfuehrung der Stromversorgung
- etwas elastisches Material wie PE-Schaum zwischen Deckel und Sensor sowie Platine, um diese mit etwas Druck gegen die Gehaeuseunterseite beim Zusammenschrauben zu fixieren

Ein paar Fotos vom Aufbau:

Einschmelzmuttern im Deckel
![Deckel](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/deckel.JPG)

Deckel und Innengehaeuse vor Montage
![Deckel und Innengehaeuse](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/deckel_und_innengehaeuse.jpg)

Deckel und Innengehaeuse montiert
![Deckel und Innengehaeuse montiert](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/innengehaeuse_montiert.JPG)

Blick von unten ins komplett montierte Gehaeuse
![HB-UNI-Sen-PM von unten](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/komplett_unten.JPG)

Hinweis: Vor Einschieben der Platine ins Gehaeuse ist der O-Ring ueber der entsprechenden Gehaeuseoeffnung zu plazieren und dann die Platine sehr vorsichtig einzuschieben, um ein Verrutschen des O-Rings zu vermeiden. An der Unterseite des Deckels sollten zwei Stuecke flexiblem Kunststoffes, z.B. PE-Schaum zu Verpackungszwecken, mit doppelseitigem Klebeband fixiert werden. Diese muessen hoeher als der Auflagerand fuer das Innengehaeuse sein, um beim Zusammenschrauben der Gehaeuseteile Druck auf SPS30 und Platine auszuueben, damit der SPS30 auf den Luftoeffnungen fest aufliegt und der SHT31 auf dem O-Ring. Zusaetzlicher Wetterschutz der Luftoeffnungen kann durch Aufschieben eines Stuecks Schlauch mit 11mm Innendurchmesser erreicht werden. Diese [Schlauchstuecken](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/3D_Druck/LuftSchlauch.stl) koennen auch aus TPU gedruckt werden.
Die uebrigen Druckteile sollten aus hinreichend wetterfestem Kunststoff wie PETG oder ASA gedruckt werden.


## Bedienung und Funktion

Das Geraet besitzt zwei wesentliche Konfigurationsparameter, die Messdauer und das Sendeintervall. Liegt die relative Luftfeuchte zu Beginn eines Messzyklus im laut Datenblatt fuer den SPS30 zulaessigen Bereich, also nicht ueber 95%, wird der SPS30 gestartet und fuer die Messdauer ohne Auslesen von Messwerten zwecks Stabilisierung betrieben. Anschliessend wird ueber den Zeitraum der Messdauer sekuendlich ein Messdatensatz vom SPS30 geliefert. Ueber diese Messdatensaetze wird der Mittelwert als Ausgabewert des Geraetes gebildet. Anschliessend wird der SPS30 abgeschaltet, das Geraet wartet bis zum Ende des Sendeintervalls und startet den naechsten Messzyklus. Details zu diesem Vorgehen sind [hier](https://sensirion.com/media/documents/188A2C3C/6166F165/Sensirion_Particulate_Matter_AppNotes_SPS30_Low_Power_Operation_D1.pdf) nachzulesen.
Liegt zu Beginn des Messzyklus die relative Feuchte ueber 95%, erfolgt keine Feinstaubmessung und in der WebUI bleiben bei Aktualisierung von Temperatur und Feuchte die Feinstaubwerte unveraendert. Dies wird durch den Hinweis auf nicht aktuelle Feinstaubwerte kenntlich gemacht.

Ein Sendeintervall kleiner als 2x Messdauer ist mithin nicht sinnvoll. Solche Einstellungen fuehren zu einem Sendeintervall etwas oberhalb von 2x Messdauer unabhaengig von der Einstellung des Sendeintervalls.

Der Sensor sollte dauerhaft mit 5V versorgt werden. Batterieversorgung ist auf Grund der relative hohen Stromaufnahme des SPS30 nicht sinnvoll. Ein hoeherfrequentes Ein- und Ausschalten der Betriebsspannung ist nicht sinnvoll, da das Geraet alle 5 Tage eine Luefterreinigung des SPS30 durchfuehrt. Zur Minimierung des Stromverbrauchs und insb. Vermeidung von Eigenerwaermung im Gehaeuse wird der ATMega nach jedem Messzyklus in den sleep mode versetzt. Geaenderte Konfigurationsparameter werden deshalb erst mit dem naechsten Messzyklus an das Geraet uebertragen.


Die Konfigurationsseite:

![WebUI Settings](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/webui_config.jpg)

Die Messwerte werden dann so angezeigt:

![WebUI Status](https://github.com/HMSteve/HB-UNI-Sen-PM/blob/main/images/webui_status.jpg)

Dabei ist zu beachten, dass die Werte kumulativ sind. PM10 enthaelt also alle Partikel <=10 µm und schliesst damit die Werte der kleineren Partikelgroessen mit ein.

## Disclaimer

Die Nutzung der hier veroeffentlichten Inhalte erfolgt vollstaendig auf eigenes Risiko und ohne jede Gewaehr.


## Lizenz

**Creative Commons BY-NC-SA**<br>
Give Credit, NonCommercial, ShareAlike

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
