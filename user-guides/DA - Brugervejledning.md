# Brugervejledning hue-strøm-overvågning

## Første opsætning

### Opsætning af Wi-Fi og valg af rum
1. Tænd for enheden. LED'en vil lyse **lilla** når den er klar.
2. Forbind din telefon eller computer til Wi-Fi-netværket med navnet `PowerPriceMonitor`. Browseren bør åbne automatisk.
3. Indtast dine Wi-Fi-oplysninger, og vælg et **Hue rumnavn** (Dette rum vil følge strømpriserne).
4. I browseren kan du konfigurere både **farveindstillinger** og **strømprisintervaller**:
   - Standardfarver:
     - **Rød**: Høj elpris
     - **Gul**: Mellem elpris
     - **Grøn**: Lav elpris
     - **Hvid**: Meget lav elpris
   - Standardprisintervaller:
     - Over 1,0 DKK/kWh: Høj elpris
     - 0,65 - 1,0 DKK/kWh: Mellem elpris
     - 0,2 - 0,65 DKK/kWh: Lav elpris
     - Under 0,2 DKK/kWh: Meget lav elpris
5. Enheden vil genstarte og forsøge at oprette forbindelse til dit Wi-Fi.

### Forbindelse til Hue Bridge
1. LED'en blinker **rød og blå skiftevis**.
2. Tryk på **knappen på din Hue Bridge** inden for 30 sekunder, for at forbinde enheden.
3. Når forbindelsen er fuldført, stopper LED'en med at blinke, og enheden begynder at styre lysene baseret på elprisen.

---


## Nulstilling af enheden

1. For at nulstille Wi-Fi-oplysninger eller andre indstillinger skal du trykke og holde **Knappen** nede i mindst **4 sekunder**, mens enheden starter.
2. Enheden genstarter for at blive konfigureret på ny.

---

## LED-indikatorer

- **Lilla (fast)**: Enheden er klar til at blive forbundet til Wi-Fi.
- **Rød og blå (blinkende skiftevis)**: Venter på godkendelse af Hue Bridge (tryk på knappen på hue bridge).
- **Andre farver**: Enheden viser strømpriser ved hjælp af de valgte farver (f.eks. Rød, Gul, Grøn, Hvid).