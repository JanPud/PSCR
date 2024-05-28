#Programowanie Systemów Czasu Rzeczywistego
Długoterminowe logowanie danych pogodowych oraz danych o produkcji, eksporcie i imporcie energii elektrycznej
##Stanowisko C1 
Odczyt danych pogodowych (temperatura, nasłonecznienie, wiatr) z serwisu https://openweathermap.org/ za pomocą REST API. Odczyt danych z terenu całej Polski w siatce o wymiarach maksymalnych 50 km x 50 km. Zadania odczytu wyzwalane za pomocą zegara (UWAGA: serwis ma maksymalnie 1000 odczytów na godzinę). Z każdym zadaniem odczytu związane zadanie wysyłania danych do stanowiska C2 – zadania synchronizowane semaforami (w szczególności semafor synchronizuje dostęp do bufora z danymi dla zdania odczytu i zadania wysyłania).
