# Programowanie Systemów Czasu Rzeczywsitego

Długoterminowe logowanie danych
pogodowych oraz danych o produkcji,
eksporcie i imporcie energii elektrycznej

## Stanowisko C1
Odczyt danych pogodowych (temperatura, nasłonecznienie, wiatr) z
serwisu https://openweathermap.org/ za pomocą REST API. Odczyt danych z terenu
całej Polski w siatce o wymiarach maksymalnych 50 km x 50 km. Zadania odczytu
wyzwalane za pomocą zegara (UWAGA: serwis ma maksymalnie 1000 odczytów na
godzinę). Z każdym zadaniem odczytu związane zadanie wysyłania danych do stanowiska
C2 – zadania synchronizowane semaforami (w szczególności semafor synchronizuje
dostęp do bufora z danymi dla zdania odczytu i zadania wysyłania).
## Stanowisko C2
Odczyt danych pogodowych ze stanowiska C2 oraz obliczenie średnich
temperatur, nasłonecznienia oraz wiatru dla całej Polski. Zadania odczytu oraz zadanie
obliczeniowe synchronizować mutexami (mutex chroni dostęp do bufora z danymi).
Nieobrobione dane pomiarowe oraz obliczone średnie przesłać do stanowiska C4.
## Stanowisko C3
Odczyt pracy polskiej sieci energetycznej (zapotrzebowanie na moc,
generacja z różnych źródeł, wymiana z krajami ościennymi, częstotliwość) z serwisu
https://www.pse.pl/home . W celu odczytu wykorzystać techniki tzw. web scraping.
Odczytane dane wysyłać okresowo do stanowiska C4. Zadanie odczytu i wysyłania
synchronizować semaforem.
## Stanowisko C4
Odbierać dane ze stanowisk C2 oraz C3 i gromadzić je w kolejce FIFO.
Dane z FIFO okresowo zapisywać do wybranej sieciowej bazy danych (zadanie zapisu
pobudzane zegarem).
