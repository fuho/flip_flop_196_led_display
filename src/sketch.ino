// Constant definitions #######################################################
#define DEBUG true
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
boolean SLOW_DOWN = false;
 
// Setup ######################################################################
void setup() {
    DDRD = DDRD | B11111100;
    DDRB = DDRB | B00111111;
    reset_screen();
    // Initialize input delay selector
    pinMode(A1,INPUT);
    digitalWrite(A1, HIGH); //init pullup
    if(digitalRead(A1)){
        SLOW_DOWN = true;
    }
    // Initialize serial port
    Serial.begin(115200);
    // Fill every byte of video RAM with structureed image data
    //fill_with_gradients();
    //fill_with_white();
    test_fill();
    video_ram_out();
}

// Main Program Loop ##########################################################
void _loop() {
    //            x-tal          Rx/Tx
    //             ↓                            ↓
    //             ??VDCBA6 543210??    pixel data: 100 0000 row: 1 segment:2
    //                                  Load Pixel Data (Horizontal negative)
    PORTD =252; // ???????? 111111??    <- send LSB
    PORTB =0;   // ??000000 111111??    <- send MSB
    PORTB =8;   // ??001000 111111??    <- trigger flip-flop flip
    //PORTB =0;   // ??000000 111111??    <- cleanup (for next raising edge)

    //                                 Load up row info (Vertical positive)
    PORTD=8;    // ???????? 000001??    <- 
    PORTB=0;    // ??000000 000001??
    PORTB=32;   // ??100000 000001??
    //PORTB=0;    // ??000000 000001??    <- cleanup (for next raising edge)

    delay(200);
    reset_screen();
    
    PORTD =0;   // ???????? 000000??    <- send LSB
    PORTB =0;   // ??000000 000000??    <- send MSB
    PORTB =30;  // ??011110 000000??    <- trigger flip-flop flip
    //PORTB=0;    // ??000000 000000??    <- cleanup (for next raising edge)

    //                                 Load up Pixel Data (Vertical positive)
    PORTD=252;  // ???????? 111111??    <- 
    PORTB=1;    // ??000001 111111??
    PORTB=33;   // ??100001 111111??

    delay(200);
    reset_screen();
    
    delay(200);
}
void loop() {
    reset_screen();
    draw_segment();
    iterate();
    if(SLOW_DOWN){
        delayMicroseconds(1000);
    }
}
// Function definitions #######################################################

// ############################################################################
// horizontal (left to right top to bottom) drawing procedure
void reset_screen(){
    PORTD = 252;    // set LSB to none
    PORTB = 1;      // set one last pixel to none
    PORTB = 31;     // burn it to 4 horozontal flip-flops
    PORTB = 0;
}
void draw_segment(){
    //            x-tal          Rx/Tx
    //             ↓                            ↓
    //             ??VDCBA6 543210??    pixel data: 100 0000 row: 1 segment:2
    //                                  Load Pixel Data (Horizontal negative)
    byte pixel_data = (~get_segment_data()) << 1;
    PORTD = pixel_data << 1;
    PORTB = pixel_data >> 7;
    PORTB = PINB | (2 << iter_segment);
    /*
    PORTD =252; // ???????? 111111??    <- send LSB
    PORTB =0;   // ??000000 111111??    <- send MSB
    PORTB =8;   // ??001000 111111??    <- trigger flip-flop flip
    */
    
    PORTD = 4 << iter_row;
    PORTB = 0;
    PORTB = 32;
    /*
    //                                 Load up row info (Vertical positive)
    PORTD=8;    // ???????? 000001??    <- 
    PORTB=0;    // ??000000 000001??
    PORTB=32;   // ??100000 000001??
    PORTB=0;    // ??000000 000001??    <- cleanup (for next raising edge)
    */

}
byte get_segment_data(){
    // returns a byte representing a segment at current position and time
    // time is represented by shade iterator
    //We have iter_row, iter_segment and iter_shade
    byte data;
    int pixel_i;
    for(int n = 0; n < DISPLAY_SEGMENT_WIDTH; n++ ){
        data = data << 1;
        pixel_i = DISPLAY_ROW_LENGTH * iter_row + DISPLAY_SEGMENT_WIDTH * iter_segment + n;
        if(video_ram[pixel_i] > iter_shade){
            data = data | 1;
        }
    }
    //data = data >> 1;
    if(DEBUG){
        dbg("@out segment_data: ", data);
    }
    return data;
}
void iterate(){
    if(++iter_segment > DISPLAY_COLUMN_SEGMENTS - 1){
        iter_segment = 0;
        iter_row++;
    }
    if(iter_row > DISPLAY_ROWS -1){
        iter_row = 0;
        iter_shade++;
    }
    if(iter_shade > DISPLAY_SHADES -1){
        iter_shade = 0;
    }
}

// ############################################################################
// other functions ############################################################

void fill_with_gradients(){
    byte color = DISPLAY_SHADES;
    for (int i = 0; i < VIDEO_RAM_LENGTH; i++){
        color--;
        video_ram[i] = color;
        if(! color){
            color = DISPLAY_SHADES;
        }
    }
}

void fill_with_white(){
    for (int i = 0; i<196; i++){
        video_ram[i] = byte(DISPLAY_SHADES);
    }
}

void test_fill(){
    video_ram[0] = 4;
    video_ram[1] = 3;
    video_ram[2] = 2;
    video_ram[3] = 1;
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

void dbg(char text[], word value){
    Serial.print(iter_shade);
    Serial.print(",");
    Serial.print(iter_row);
    Serial.print(",");
    Serial.print(iter_segment);
    Serial.print(", ");
    Serial.print(text);
    Serial.println(value, BIN);
}
