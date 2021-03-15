/* This code was taken from StackOverflow
 * https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
 */

struct termios old, current;

/* Initialize new terminal i/o settings
 * @param echo Set echo on or off
 */
void initTermios (bool echo) {
  tcgetattr(0, &old);                 // grab old terminal I/O settings
  current = old;                      // make new settings same as old settings
  current.c_lflag &= ~ICANON;         // disable buffered I/O
  if (echo) {
      current.c_lflag |= ECHO;        // set echo mode
  } else {
      current.c_lflag &= ~ECHO;       // set no echo mode
  }
  tcsetattr(0, TCSANOW, &current);    // use these new terminal I/O settings now
}

/* Restore old terminal i/o settings
 */
void resetTermios (void) {
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode
 */
char getch_ (int echo) {
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo
 */
char getch (void) {
  return getch_ (false);
}

/* Read 1 character with echo
 */
char getche (void) {
  return getch_ (true);
}
