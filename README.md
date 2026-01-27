# ESP32 VectorGame Console 
**Version 0.1 beta – Released January 2026**

This project transforms an analog CRT oscilloscope into a functional vector gaming console. Instead of using pixels, the ESP32 utilizes its internal Digital-to-Analog Converters (DAC) to drive the electron beam of an oscilloscope directly.

The result is a crisp, glowing retro-aesthetic, reminiscent of the legendary **Vectrex** or classic arcade machines like *Asteroids*.

---

##  How it Works
The console operates by using the **XY-Mode** of an oscilloscope:
*   **Vector Rendering:** Objects are drawn sequentially as lines. The ESP32 outputs varying voltages (0-3.3V) to position the beam on the X and Y axes.
*   **Blanking Simulation:** To avoid "ghost lines" while moving the beam between separate objects, the engine increases the beam's movement speed significantly during "jumps," making the transitions nearly invisible.
*   **Optimized Timing:** The code uses `inline` functions and precise `delayMicroseconds` to ensure a stable, flicker-free image on the CRT.

##  Included Games & Features
*   **Menu System:** Select games using a potentiometer and a fire button.
*   **Vector-Pong:** Classic 2-player paddle game.
*   **Vector-Invaders:** A space shooter clone with smooth vector animations and sound effects.
*   **Vector Font Engine:** Custom-built routines to render text (e.g., "FH KAERNTEN") and numbers.
*   **Sound:** Integrated PWM-based audio for gameplay feedback.

##  Hardware Setup
### Requirements
1.  **ESP32** (e.g., DevKit V1)
2.  **Analog Oscilloscope** with XY-mode capability.
3.  **Controllers:** 2x Potentiometers (for paddles/menu), 1x Push button (Fire).
4.  **Audio:** A Piezo buzzer or small speaker.

### Pinout Configuration
| Component | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **DAC X-Axis** | GPIO 25 | Horizontal deflection (Scope CH1) |
| **DAC Y-Axis** | GPIO 26 | Vertical deflection (Scope CH2) |
| **Poti Left** | GPIO 34 | Player 1 / Menu Navigation |
| **Poti Right** | GPIO 35 | Player 2 |
| **Fire Button** | GPIO 32 | Start Game / Shoot (Active LOW) |
| **Sound Out** | GPIO 27 | PWM Audio Output |

##  Installation
1.  Connect the ESP32 DAC pins (25 & 26) to your oscilloscope probes. **Ensure a common Ground (GND).**
2.  Set your oscilloscope to **XY-Mode**.
3.  Open the code in the [Arduino IDE](https://www.arduino.cc).
4.  Install the ESP32 Board-Package and upload the sketch.
5.  Adjust the `VOLTS/DIV` settings on your scope to fit the playing field to the screen.

##  Roadmap
- [ ] Implement true Z-Axis blanking for supported oscilloscopes.
- [ ] PCB for more options and easy to use
- [ ] Add more game modules (e.g., Asteroids clone).
- [ ] Optimization of the `drawLine` algorithm for even higher refresh rates.

---
**Author:**  ([ingmarsretro](https://github.com))  
*Created for a demonstration at the FH-Day, January 2026. Stay retro!*


# ESP32 VectorGame Console 🕹️
**Version 0.1 Beta – Stand: Januar 2026**

Dieses Projekt verwandelt ein gewöhnliches analoges Oszilloskop in eine voll funktionsfähige Vektor-Spielekonsole. Anstatt Pixel auf einem modernen Display anzuzeigen, nutzt der ESP32 seine internen Digital-Analog-Wandler (DAC), um den Elektronenstrahl eines Oszilloskops im XY-Modus direkt zu steuern.

Das Ergebnis ist der authentische, glühende Retro-Look der 80er Jahre, bekannt von der **Vectrex**-Konsole oder Arcade-Klassikern wie *Asteroids*.

---

##  Funktionsweise
Die Konsole nutzt den **XY-Modus** eines Oszilloskops:
*   **Vektorgrafik:** Alle Objekte werden sequenziell als Linien gezeichnet. Der ESP32 gibt variable Spannungen (0-3.3V) an zwei Pins aus, um den Strahl auf der X- und Y-Achse zu positionieren.
*   **Blanking-Simulation:** Um unerwünschte Verbindungslinien ("Geisterlinien") zwischen separaten Objekten zu vermeiden, erhöht die Engine die Bewegungsgeschwindigkeit des Strahls bei Sprüngen massiv. Dadurch werden diese Linien für das menschliche Auge fast unsichtbar.
*   **Performance:** Durch optimierte `inline`-Funktionen und präzises Timing via `delayMicroseconds` wird ein flimmerfreies Bild auf der CRT-Röhre erzeugt.

##  Enthaltene Spiele & Features
*   **Menü-System:** Spielauswahl über ein Potentiometer und einen Start-Button.
*   **Vector-Pong:** Der Klassiker für zwei Spieler mit flüssiger Paddle-Steuerung.
*   **Vector-Invaders:** Ein Space-Shooter-Klon mit Vektor-Animationen und Soundeffekten.
*   **Custom Vector Fonts:** Eigens entwickelte Routinen zum Zeichnen von Text (z.B. "FH KAERNTEN") und Ziffern für Spielstände.
*   **Sound:** Integrierte PWM-Tonausgabe für akustisches Feedback während des Spiels.

##  Hardware-Setup
### Anforderungen
1.  **ESP32** (z.B. DevKit V1)
2.  **Analoges Oszilloskop** mit XY-Modus-Funktion.
3.  **Controller:** 2x Potentiometer (10k Ohm für Steuerung), 1x Taster (Feuer/Start).
4.  **Audio:** Ein Piezo-Buzzer oder kleiner Lautsprecher.

### Pinbelegung
| Komponente | ESP32 Pin | Funktion |
| :--- | :--- | :--- |
| **DAC X-Achse** | GPIO 25 | Horizontale Ablenkung (Scope CH1) |
| **DAC Y-Achse** | GPIO 26 | Vertikale Ablenkung (Scope CH2) |
| **Poti Links** | GPIO 34 | Spieler 1 / Menü-Navigation |
| **Poti Rechts** | GPIO 35 | Spieler 2 |
| **Fire Button** | GPIO 32 | Start / Schießen (Active LOW) |
| **Sound Out** | GPIO 27 | PWM Audio-Ausgang |

##  Installation
1.  Verbinde die DAC-Pins (25 & 26) des ESP32 mit den Tastköpfen deines Oszilloskops. **Achte auf eine gemeinsame Masse (GND).**
2.  Stelle das Oszilloskop in den **XY-Modus**.
3.  Öffne den Code in der [Arduino IDE](https://www.arduino.cc).
4.  Installiere das ESP32-Board-Paket und lade den Sketch hoch.
5.  Nutze die `VOLTS/DIV`-Regler am Oszilloskop, um das Spielfeld auf die Bildschirmgröße anzupassen.

##  Roadmap
- [ ] Implementierung von echtem Z-Achsen-Blanking für professionelle Oszilloskope.
- [ ] Eine Platine die mehrere optionen bietet (externen dac, bnc anschlusse ... )
- [ ] Hinzufügen weiterer Module (z.B. Asteroids-Klon).
- [ ] Optimierung des `drawLine`-Algorithmus für noch höhere Bildwiederholraten.

---
**Autor:** Ingmar Bihlo ([ingmarsretro](https://github.com))  
*Erstellt für eine Demonstration am FH-Tag im Januar 2026. Stay retro!*
