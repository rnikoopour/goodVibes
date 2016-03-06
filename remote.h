#ifndef REMOTE
#define REMOTE

#define on 0xF7C03F
#define red1 0xF7609F
#define red2 0xF750AF
#define red3 0xF7708F
#define red4 0xF748B7
#define red5 0xF76897
#define green1 0xF720DF
#define green2 0xF710EF
#define green3 0xF730CF
#define green4 0xF708F7
#define green5 0xF728D7
#define blue1 0xF7A05F
#define blue2 0xF7906F
#define blue3 0xF7B04F
#define blue4 0xF78877
#define blue5 0xF7A857
#define off 0xF740BF

#define onBtn 0
#define red1Btn 1
#define red2Btn 2
#define red3Btn 3
#define red4Btn 4
#define red5Btn 5
#define green1Btn 6
#define green2Btn 7
#define green3Btn 8
#define green4Btn 9
#define green5Btn 10
#define blue1Btn 11
#define blue2Btn 12
#define blue3Btn 13
#define blue4Btn 14
#define blue5Btn 15
#define offBtn 16

long buttons[] = {
  on,
  red1,
  red2,
  red3,
  red4,
  red5,
  green1,
  green2,
  green3,
  green4,
  green5,
  blue1,
  blue2,
  blue3,
  blue4,
  blue5,
  off
};

char numButtons = 17;

void pressButton(IRsend *My_Sender, int button) {
  My_Sender->send(NEC, buttons[button], 32);
}

#endif
