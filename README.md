## Informacje o projekcie
Twórcy: Jan Metzler 148137 I6.1, Mateusz Poprawa 148073 I6.2

Temat projektu: imitacja gry Kahoot 


## Początek
Gracz łączy się z serwerem i wybiera czy chce utworzyć quiz czy dołączyć do istniejącego.
Po połączeniu gracz wybiera swój nick, który będzie go identyfikował w trakcie rozgrywki. 
Jeżeli wybierze nick już zajęty przez innego użytkownika serwer poprosi o ponowne wybranie nicku.

## Tworzenie quizu
Gracz po wybraniu opcji utworzenia quizu wprowadza pytania oraz 4 przykładowe odpowiedzi, zaznaczając która z nich jest poprawna.
Na koniec procesu tworzenia quizu gracz otrzymuje kod dostępu pozwalający innym użytkownikom na dołączenie.
Gracz po wystartowaniu quizu widzi na bieżąco ile osób wybrało poszególne odpowiedzi.

## Uczestnictwo w quizie
Gracz po wybraniu opcji dołączenia do istniejącego quizu wpisuje kod i dołącza do lobby. 
W przypadku błędnego kodu serwer prosi o podanie go ponownie.  
Gracz widzi w lobby innych graczy i może w każdej chwili opuścić grę.
Po uruchomieniu rozgrywki na ekranie uczestników wyświetlają się pytania oraz możliwe odpowiedzi, wśród których gracze muszą wybrać jedną.
Uczestnicy widzą wzajemną punktację. 

## Szczegóły implementacyjne
Ilość pytań oraz czas na odpowiedź można ustalić od strony serwera za pomocą zmiany odpowiednich plików .ini. Możliwośc udzielenia odpowiedzi może stać się niedostępna przed upływem wyznaczonego czasu, pod warunkiem, że 2/3 graczy biorących udział w rozgrywce udzieli swoich odpowiedzi. 
Za każdą poprawną odpowiedź naliczane są punkty liczone wzorem 1000 * x, gdzie x to % pozostałego na pytanie czasu w momencie udzielenia odpowiedzi.
Serwer obsługuje wiele quizów naraz. 
Po wyczerpaniu się listy pytań wyświetlany jest zwycięzca.
