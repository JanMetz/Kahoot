## Informacje o projekcie
Twórcy: Jan Metzler 148137 I6.1, Mateusz Poprawa 148073 I6.2

Temat projektu: imitacja gry Kahoot 


## Początek
Gracz łączy się z serwerem i wybiera czy chce utworzyć quiz czy dołączyć do istniejącego.

## Tworzenie quizu
Gracz wybiera swój nick, który będzie go identyfikował w trakcie rozgrywki.
Gracz wprowadza pytania oraz 4 przykładowe odpowiedzi, zaznaczając która z nich jest poprawna.
Na koniec procesu tworzenia quizu gracz otrzymuje kod dostępu pozwalający innym użytkownikom na dołączenie.

## Uczestnictwo w quizie
Gracz po wybraniu opcji dołączenia do istniejącego quizu wpisuje kod oraz swój nick i dołącza do lobby. 
W przypadku błędnego kodu lub wpisania już zajętego nick'a serwer prosi o ponowne podanie błędnej danej(danych).  
Gracz widzi w lobby innych graczy i może w każdej chwili opuścić grę.
Po uruchomieniu rozgrywki na ekranie uczestników wyświetlają się pytania oraz możliwe odpowiedzi, wśród których gracze muszą wybrać jedną.
Uczestnicy widzą wzajemną punktację. 

## Szczegóły implementacyjne
Ilość pytań oraz czas na odpowiedź są ustalane przez użytkownika tworzącego quiz. Możliwośc udzielenia odpowiedzi może stać się niedostępna przed upływem wyznaczonego czasu, pod warunkiem, że 2/3 graczy biorących udział w rozgrywce udzieli swoich odpowiedzi. 
Za każdą poprawną odpowiedź naliczane są punkty liczone wzorem 1000 * x, gdzie x to % pozostałego na pytanie czasu w momencie udzielenia odpowiedzi.
Serwer obsługuje wiele quizów naraz. 
Po wyczerpaniu się listy pytań wyświetlany jest zwycięzca.

## Serwer - instalacja (Linux)
```
g++ -std=c++17 -o server.exe main.cpp server.cpp game.cpp questions.cpp -lpthread

```

## Klient - instalacja (Linux)
```
cmake -DCMAKE_PREFIX_PATH=~/YOUR_PATH/Qt/gcc_64/ -S Kahoot_client -B build
cd build
make
./Kahoot_client

```
