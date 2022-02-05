# HB-UNI-Sen-PM

![HB-UNI-Sen-CO2](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/dev_front.jpg)

Eine AskSinPP-Implementierung eines Feinstaubsensors fuer den Aussenbereich mit dem Feinstaubsensor SPS30 sowie dem Feuchtesensor SHT31 von Sensirion. Das Geraet kann regulaer in eine Homematic-Umgebung eingebunden werden und liefert folgende Messwerte:

- Feinstaubkonzentrationen PM1, PM2.5, PM4 und PM10 im µg/m³
- Partikelamzahlen je cm³ fuer Partikelgroessen <0.5µm, <1µm, <2.5µm, <4µm und <10µm
- Temperatur
- Lueftfeuchte






## Platinen

Auf der Platine sind drei I2C-Steckplaetze fuer Sensoren vorgesehen. Neben dem [Sensirion SCD30](https://www.sensirion.com/de/umweltsensoren/kohlendioxidsensor/kohlendioxidsensoren-co2/) bietet sich ein [4-poliges BME280-Modul](https://www.ebay.de/itm/BME280-Temperatur-Luftdruck-Feuchtigkeit-Sensor-I2C-1-8-5V-Modul/114603492524) zur Luftdruck-Kompensation an. Der Drucksensor ist auch in der Firmware vorgesehen. Der dritte Steckplatz ist aktuell frei.
Als Display findet ein [1.54" Waveshare-E-Paper-Modul](https://www.waveshare.com/1.54inch-e-Paper-Module.htm) Verwendung.

[Schaltplan](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/PCB/HB-Uni-Sen-CO2_Schematic.pdf)

[Stueckliste](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/PCB/HB_UNI_Sen_CO2_BOM.pdf)

Ein paar Fotos vom Aufbau:

![PCB Top](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/pcb_top_1.jpg)

![PCB Bottom](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/pcb_bot_1.jpg)

![PCB Top 2](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/pcb_top_2.jpg)

![PCB Bottom 2](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/pcb_bot_2.jpg)

### Bestueckung

Die Bestueckung der Hauptplatine ist unkritisch und kann per Hand erfolgen, es werden Bauformen 0805 oder groesser genutzt. Bei der Feuchtesensorplatine ist der SHT31 zuerst zu bestuecken. Grundsaetzlich ist Handloetung unter guter Lupe oder Mikroskop moeglich, vorteilhaft ist jedoch der Auftrag von Lotpaste mittels Schablone und Loetung mit Hitze, bspw Heissluft, von unten durch die Platine. Bei diesem Verfahren ist keine 100% exakte Positionierung des Bauteils noetig, da es bei korrekt dosierter Lotpaste nach deren Aufschmelzen auf Grund der Oberflaechenspannung auf dem flüssigen Lot von selbst in die korrekte Position schwimmt. Lupe oder Mikroskop zur Komtrolle der Loetstellen von der Seite sollte man nutzen. Anschliessend wird die Schutzkapp in die vorgesehenen Loecher eingesteckt und haelt ohne Klebstoffe etc.
Die beiden Platinen sollten erst ganz zum Schluss, ggf nach Waschen der Hauptplatine, zusammengesteckt und verloetet werden. Der SHT31 darf weder Waschfluessigkeit wie Isopropanol noch deren Daempfen ausgesetzt werden. Anderenfalls ist mit einer (ohne Backen) irreversible Fehlmessung der Luftfeuchte zu rechnen. Eine k


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
- ein O-Ring 5mm x 1, moeglich ist auch 6mm x 1.5 oder aehnlich, zum Abichten der SF2-Filterkappe ueber dem SHT31 gegen das Gehaeuse
- vier Schrauben M3 x 10, Edelstahl oder Messing, zum Montieren des inneren Gehaeuse am Deckel
- EPDM-Dichtschnur 3mm zum Abdichten des inneren Gehaeuses gegen den Deckel
- vier Gewindestange M8 zzgl. vier Muttern M8 zum Zusammenbau von Deckel, Zwischenringen und Haltewinkel, Edelstahl oder Messing
- eine PG-Verschraubung M12 zur Einfuehrung der Stromversorgung
- etwas elastisches Material zwischen Deckel und Sensor sowie Platine, um diese mit etwas Druck gegen die Gehaeuseunterseite beim Zusammenschrauben zu fixieren


## Bedienung

Es gibt neben der AskSinPP-Config-Taste eine weitere Bedientaste. Ein kurzer Druck auf diese schaltet die Ampel-LED ein bzw. aus, ein langer Druck fuehrt zu einer forced calibration des SCD30. Hierzu sollte sich der Sensor bereits einige Minuten an der frischen Luft ohne nennenswerte Druckschwankungen durch Wind befunden haben. Der lange Tastendruck kalibriert dann den Sensor auf den im WebUI hinterlegten CO2-Referenzwert (im Freien ca. 410ppm). Die automatische Kalibrierung wird nicht verwendet auf Basis der These, dass ein Wegdriften der Auto-Kalibrierung bei nicht ausreichendem regelmaessigem Lueften problematischer ist als die Alterungsdrift nach forced calibration. Langzeitbeobachtungen hierzu fehlen mir jedoch.

Unterschreitet die Akkuspannung 2.2V, wird ein Warnsymbol im Display angezeigt und ein USB-Ladegerate sollte angeschlossen werden. Geschieht das nicht und die Spannung sinkt unter 2.0V, schaltet sich der Sensor zum Schutz vor Tiefentladung ab und zeigt dies an. Ein Reset (Wiedereinschalten) erfolgt automatisch beim Anschluss eines Ladegeraetes. Die gruene LED zeigt das Anliegen der Ladespannung, die gelbe den Schnelladevorgang. Es ist zu beachten, dass die Erwaermung beim Schnelladen natuerlich die Messwerte im Gehaeuse verfaelscht.

Die Schaltschwellen der Ampelfarben, die Hoehe ueber NN sowie ein vom SCD30-Temperaturmesswert zu subtrahierender Offset zur Korrektur der Anzeige koennen im WebUI konfiguriert werden.

![WebUI Settings](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/webui_settings.jpg)

Die Messwerte werden dann so angezeigt:

![WebUI Status](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/webui_status.jpg)

### Hinweise zum Energieverbrauch

Der SCD30 zieht einen nennenswerten Ruhestrom (mind. 5mA). Ein Power Cycling wird vom Hersteller nicht empfohlen, siehe [hier](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Addl/CD_AN_SCD30_Low_Power_Mode_D2.pdf).
Damit ist fuer das Geraet bei der Voreinstellung einer Sensor-Abtastperiode von 16s eine Akkulaufzeit von hoechstens 5 Tagen zu erwarten. Das heisst, der mobile Betrieb waehrend eines oder einiger (Arbeits)tage ist problemslos moeglich. Der Dauerbetrieb sollte jedoch eher mit USB-Netzteil erfolgen. Dabei versorgt nach Ende der Schnelladung der von der MAX712-Ladeschaltung gelieferte, von R20 gemaess [Datenblatt MAX712](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Addl/MAX712-MAX713.pdf) feinjustierte Erhaltungsladestrom das Device.  

Hier eine Messung zum Stromverbrauch:

![Power Consumption Detail](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Images/power_consumption_detail.jpg)

Ein Excel-Workbook zum power budgeting findet sich [hier](https://github.com/HMSteve/HB-UNI-Sen-CO2/blob/main/Addl/BatteryBudgeting.xlsx).


## Gehaeuse

Zwischen Display und Platine sind vier zu druckende Abstandshuelsen einzusetzen. Das Gehaeuse stellt beim Drucken keine groesseren Herausforderungen. Die LED-Loecher sind ggf. mit 10.5mm bzw. 3.5mm aufzubohren, die Loecher im Boden mit 3mm.
Fuer die Befestigung der Platine sind [M3-Einpressmuttern](https://www.amazon.de/dp/B08BCRZZS3) vorgesehen. Die Befestigung der Platine im Gehaeuse erfolgt mit M3x10 Distanzbolzen. Die Rueckwand wird dann mit M3x10 Zylinderkopfschrauben an diesen befestigt.


## Disclaimer

Die Nutzung der hier veroeffentlichten Inhalte erfolgt vollstaendig auf eigenes Risiko und ohne jede Gewaehr.


## Lizenz

**Creative Commons BY-NC-SA**<br>
Give Credit, NonCommercial, ShareAlike

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
