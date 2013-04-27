// Constant definitions #######################################################
#define DEBUG false
#define VIDEO_RAM_LENGTH 196
#define VERTICAL_SEGMENT_ID 4
#define DISPLAY_ROWS 7
#define DISPLAY_COLUMN_SEGMENTS 4
#define DISPLAY_SHADES 4
#define DISPLAY_SEGMENT_WIDTH 7
#define DISPLAY_ROW_LENGTH 28

// Global variable declarations ###############################################
byte video_ram[196];                // Display image data storage
int iter_shade      = 0,            // Colour(shade) counter,
    iter_row        = 0,            // Horizontal line counter
    iter_segment    = 0;            // Vertical segment row counter

// Setup ######################################################################
void setup() {
    DDRD = DDRD | B11111100;
    DDRB = DDRB | B00111111;


    // Initialize serial port
    Serial.begin(115200);
    // Fill every byte of video RAM with structureed image data
    fill_with_gradients();
    //fill_with_white();
    
    video_ram_out();
}

// Main Program Loop ##########################################################
void loop() {
    //do stuff here
    //delayMicroseconds(1);
    h_draw();
}

// Function definitions #######################################################

// ############################################################################
// horizontal (left to right top to bottom) drawing procedure
void h_draw(){
    h_erase_screen();
    h_draw_segment();
    h_iterate();
}
void h_iterate(){
    iter_segment++;
    if(iter_segment >= DISPLAY_COLUMN_SEGMENTS){
        iter_segment = 0;
        iter_row++;
    }
    if(iter_row >= DISPLAY_ROWS){
        iter_row = 0;
        iter_shade++;
    }
    if(iter_shade >= DISPLAY_SHADES){
        iter_shade = 0;
    }
}
void h_draw_segment(){
    h_prepare_segment(iter_segment, h_get_segment_data());
    h_display_segment();
}
void h_erase_screen(){
    /*byte lsb = ~0 << 1;
    byte msb = 0;
    word w = word(msb, lsb);  // unite bytes to one word
    w = w << 1;
    // prepare values to pins
    PORTD = lowByte(w);
    PORTB = highByte(w);
    // trigger flip flop read + output
    PORTB = ~0;
    */
    //  Port B   Port D
    // --VDCBAd dddddd--
    // ??000000 000000??
    //     0       0
    PORTD=0;
    PORTB=0;
}
byte h_get_segment_data(){
    // returns a byte representing a segment at current position and time
    // time is represented by shade iterator
    //We have iter_row, iter_segment and iter_shade
    byte data = 0;
    int pixel_i;
    for(int n = 0; n < DISPLAY_SEGMENT_WIDTH; n++ ){
        data = data << 1;
        pixel_i = DISPLAY_ROW_LENGTH * iter_row + DISPLAY_SEGMENT_WIDTH * iter_segment + n;
        if(video_ram[pixel_i] > iter_shade){
            data = data | 1;
        }
    }
    if(DEBUG){
        serial_report(data,"");
    }
    return data;
}
void h_prepare_segment(int seg_pos, byte seg_data){
    // example: (int)2, (byte)01110101 (###_#_#)  # is lit up
    // XX______ ______XX
    //          01110101       << 1
    //          11101010
    // 00000001                << 2
    // 00000100
    // 00
    byte lsb = ~seg_data << 1;
    byte msb = 0;
    word w = word(msb, lsb);  // unite bytes to one word
    w = w << 1;
    // prepare values to pins
    PORTD = lowByte(w);
    PORTB = highByte(w);
    // trigger flip flop read + output
    PORTB = 2 << seg_pos;
}
void h_display_segment(){
    // Draws row at defined position ()
    // 0 = 11111110
    // 1 = 11111101
    // 2 = 11111011
    // ..
    // 6 = 10111111
  h_prepare_segment(VERTICAL_SEGMENT_ID, ~(1 << iter_row));
}

// ############################################################################
// vertical (left to right top to bottom) drawing procedure ###################
void v_draw(){
    v_erase_segment();
    v_draw_segment();
    v_iterate();
}
void v_iterate(){
    iter_segment++;
    if(iter_segment >= DISPLAY_COLUMN_SEGMENTS){
        iter_segment = 0;
        iter_row++;
    }
    if(iter_row >= DISPLAY_ROWS){
        iter_row = 0;
        iter_shade++;
    }
    if(iter_shade >= DISPLAY_SHADES){
        iter_shade = 0;
    }
}
void v_draw_segment(){
    v_prepare_segment(iter_segment, v_get_segment_data());
    v_display_segment();
}
void v_erase_segment(){
    v_prepare_segment(iter_segment, 0);
    v_display_segment();
}
void v_prepare_segment(int seg_pos, byte seg_data){
  // example: (int)2, (byte)01110101 (###_#_#)  # is lit up
  // XX______ ______XX
  //          01110101       << 1
  //          11101010
  // 00000001                << 2
  // 00000100
  // 00
  byte lsb = ~seg_data << 1;
  byte msb = 0;
  word w = word(msb, lsb);  // unite bytes to one word
  w = w << 1;
  // prepare values to pins
  PORTD = lowByte(w);
  PORTB = highByte(w);
  // trigger flip flop read + output
  PORTB = 2 << seg_pos;
}
void v_display_segment(){
    // Draws row at defined position ()
    // 0 = 11111110
    // 1 = 11111101
    // 2 = 11111011
    // ..
    // 6 = 10111111
  v_prepare_segment(4, ~(1 << iter_row));
}
byte v_get_segment_data(){
    // returns a byte representing a segment at current position and time
    // time is represented by shade iterator

    //We have iter_row, iter_segment and iter_shade
    byte data = 0;
    int pixel_i;
    for(int n = 0; n < DISPLAY_SEGMENT_WIDTH; n++ ){
        data = data << 1;
        pixel_i = DISPLAY_ROW_LENGTH * iter_row + DISPLAY_SEGMENT_WIDTH * iter_segment + pixel_i;
        if(video_ram[pixel_i] > iter_shade){
            data = data | 1;
        }
    }
    if(DEBUG){
        serial_report(data,"-");
    }
    return data;
}
// ############################################################################
// other functions ############################################################

void fill_with_gradients(){
    byte color = DISPLAY_SHADES;
    for (int i = 0; i<196; i++){
        video_ram[i] = color;
        color = color >> 1;
        if(! color){
            color = DISPLAY_SHADES;
        }
    }
}

void fill_with_white(){
    for (int i = 0; i<196; i++){
        video_ram[i] = byte(15);
    }
}

void video_ram_out(){
    for (int i =0; i < VIDEO_RAM_LENGTH; i++ ){
        if(!( i % 28)){
            Serial.println("");
        }
        Serial.print(video_ram[i],HEX);
    }
    Serial.println();
}

void serial_report(byte value, char text[] ){
    Serial.print(text);
    Serial.print(iter_segment);
    Serial.print(" ");
    Serial.print(iter_row);
    Serial.print(" ");
    Serial.print(iter_shade);
    Serial.print(" ");
    Serial.println(value, BIN);
}
