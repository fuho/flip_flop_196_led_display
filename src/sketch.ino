// Graphics Memory
byte video_ram[196];
const int DISPLAY_ROWS      = 7,
    DISPLAY_COLUMN_SEGMENTS = 4,
    DISPLAY_SHADES          = 16;

int iter_shade      = 0,
    iter_row        = 0,
    iter_segment    = 0;

void setup() {
    DDRD = DDRD | B11111100;
    DDRB = DDRB | B00111111;

    // Fill every byte of video RAM with random number between 0 and 255
    fill_with_gradients();
}

void loop() {
    //do stuff here
}

void fill_with_gradients(){
    for (int i = 0; i<196; i++){
        video_ram[i] = byte(i % 16);
    }
}

void draw_screen(){
    draw_row();
    if(iter_row == 0 && iter_segment == 0){
        if (++iter_shade >= DISPLAY_SHADES){
            iter_shade = 0;
        }
    }
}

void draw_row(){
    // Draw requested row
    draw_segment();
    if(++iter_row >= DISPLAY_ROWS){
        iter_row = 0;
    }
}

void draw_segment(){
    prepare_segment();
    display_segment();
    if(++iter_segment >= DISPLAY_COLUMN_SEGMENTS){
        iter_segment = 0;
    }
}

void prepare_segment(){
  // example: (int)2, (byte)01110101 (###_#_#)  # is lit up
  // XX______ ______XX
  //          01110101       << 1
  //          11101010
  // 00000001                << 2
  // 00000100
  // 00
  byte segment_data = get_segment_data();
  byte lsb = ~segment_data << 1;
  byte msb = 0;
  word w = word(msb, lsb);  // unite bytes to one word
  w = w << 1;
  // prepare values to pins
  PORTD = lowByte(w);
  PORTB = highByte(w);
  // trigger flip flop read + output
  PORTB = 2 << iter_segment;
}

void display_segment(){
  byte lsb = ~4 << 1;
  byte msb = 0;
  word w = word(msb, lsb);  // unite bytes to one word
  w = w << 1;
  // prepare values to pins
  PORTD = lowByte(w);
  PORTB = highByte(w);
  // trigger flip flop read + output
  PORTB = 2 << ~(1 << iter_row);
}

byte get_segment_data(){
    // returns a byte representing a segment at current position and time
    // time is represented by shade iterator

    //We have iter_row, iter_segment and iter_shade
    byte data = 0;
    int pixel_i;
    for(int n = 0; n < 8; n++ ){
        pixel_i = DISPLAY_ROWS * iter_row + DISPLAY_COLUMN_SEGMENTS * iter_segment + pixel_i;
        byte pixel = video_ram[pixel_i];
        if(pixel >= iter_shade){
            data = data | 1;
        }
        data = data << 1;
    }
    return data;
}
