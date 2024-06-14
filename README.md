# Instrukcja kompilacji całego pakietu

## Wstęp

Ten dokument zawiera instrukcje dotyczące kompilacji i uruchomienia drivera w ramach tego pakietu.

## Instrukcja kompilacji clienta

1. Przejdź do katalogu głównego projektu.
2. W terminalu otwórz folder z pakietem.
3. Skompiluj pakiet poniższą komendą i utwórz plik wykonawczy "client". Uwaga, program zawiera funkcje dostępne w
   nowszej wersji języka c++, dlatego musimy załączyć flagę -std=c++17.

```
    $ g++ -std=c++17 main.cpp checkersBoard.cpp gameHandler.cpp gameAlgorithm.cpp -o client
```

## Instrukcja połączenia przez NET

1. Przejdź do katalogu głównego projektu.
2. W terminalu otwórz folder z pakietem.
3. Skompiluj pakiet poniższą komendą i utwórz plik wykonawczy "broker".

```
    $ gcc -o broker -g -Wall -std=c17 -pedantic checkers_broker.c
```

4. Uruchom "broker" z plikiem wykonawczym clienta.

```
   $ ./broker client client
```

## Instrukcja połączenia przez GUI

1. Upewnij się, że client jest już skompilowany w poprzednim punkcie.
2. Uruchom "clienta" poniższą komendą. Wybierz kolor gracza, którym chcesz zagrać.

```
 $ ./gra GUI WHITE 8
```