// Constant definitions #######################################################
#define DEBUG false
#define VIDEO_RAM_LENGTH 196
#define VERTICAL_SEGMENT_ID 4
#define DISPLAY_SHADES 14
#define DISPLAY_ROWS 7
#define DISPLAY_COLUMN_SEGMENTS 4
#define DISPLAY_SEGMENT_WIDTH 7
#define DISPLAY_ROW_LENGTH 28
#define pass; __asm__("nop\n\t");

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

    gradient_fill();
    //white_fill();
    //test_fill();
    
    video_ram_out();
}

// Main Program Loop ##########################################################
void loop() {
    reset_screen();
    draw_segment();
    //if(SLOW_DOWN) delay(200);
    
    iterate();
}
// Function definitions #######################################################

// ############################################################################
// horizontal (left to right top to bottom) drawing procedure
void reset_screen(){
    PORTB = 1;      // set one last pixel to none
    PORTD = 252;    // set LSB to none
    PORTB = 31;     // burn it to 4 horozontal flip-flops

    pass;
    PORTB = 0;
    PORTD = 0;
    PORTB = 32;
}
void draw_segment(){
    //            x-tal          Rx/Tx
    //             ↓                            ↓
    //             ??VDCBA6 543210??    pixel data: 100 0000 row: 1 segment:2
    //                                  Load Pixel Data (Horizontal negative)
    byte pixel_data = (~get_segment_data()) << 1;
    PORTB = pixel_data >> 7;
    PORTD = pixel_data << 1;
    PORTB = PINB | (2 << iter_segment);
    
    PORTB = (2 << iter_row) >> 7;
    PORTD = 4 << iter_row;
    PORTB = PINB | 32;
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
    return data;
}
void iterate(){
    if(++iter_segment > DISPLAY_COLUMN_SEGMENTS - 1){
        iter_segment = 0;
        iter_row++;
    }
    if(iter_row > DISPLAY_ROWS - 1 ){
        iter_row = 0;
        iter_shade++;
    }
    if(iter_shade > DISPLAY_SHADES -1){
        iter_shade = 0;
    }
}

// ############################################################################
// other functions ############################################################

void gradient_fill(){
    byte color = DISPLAY_SHADES;
    for (int i = 0; i < VIDEO_RAM_LENGTH; i++){
        color--;
        video_ram[i] = color;
        if(color == 0){
            color = DISPLAY_SHADES;
        }
    }
}

void white_fill(){
    for (int i = 0; i<196; i++){
        video_ram[i] = byte(DISPLAY_SHADES);
    }
}

void test_fill(){
    video_ram[0] = 7;
    video_ram[1] = 6;
    video_ram[2] = 5;
    video_ram[3] = 4;
    video_ram[4] = 3;
    video_ram[5] = 2;
    video_ram[6] = 1;
    video_ram[7] = 0;

    video_ram[36 + 0] = 7;
    video_ram[36 + 1] = 6;
    video_ram[36 + 2] = 5;
    video_ram[36 + 3] = 4;
    video_ram[36 + 4] = 3;
    video_ram[36 + 5] = 2;
    video_ram[36 + 6] = 1;
    video_ram[36 + 7] = 0;

    video_ram[72 + 0] = 7;
    video_ram[72 + 1] = 6;
    video_ram[72 + 2] = 5;
    video_ram[72 + 3] = 4;
    video_ram[72 + 4] = 3;
    video_ram[72 + 5] = 2;
    video_ram[72 + 6] = 1;
    video_ram[72 + 7] = 0;

    video_ram[108 + 0] = 7;
    video_ram[108 + 1] = 6;
    video_ram[108 + 2] = 5;
    video_ram[108 + 3] = 4;
    video_ram[108 + 4] = 3;
    video_ram[108 + 5] = 2;
    video_ram[108 + 6] = 1;
    video_ram[108 + 7] = 0;

    video_ram[144 + 0] = 7;
    video_ram[144 + 1] = 6;
    video_ram[144 + 2] = 5;
    video_ram[144 + 3] = 4;
    video_ram[144 + 4] = 3;
    video_ram[144 + 5] = 2;
    video_ram[144 + 6] = 1;
    video_ram[144 + 7] = 0;

    video_ram[180 + 0] = 7;
    video_ram[180 + 1] = 6;
    video_ram[180 + 2] = 5;
    video_ram[180 + 3] = 4;
    video_ram[180 + 4] = 3;
    video_ram[180 + 5] = 2;
    video_ram[180 + 6] = 1;
    video_ram[180 + 7] = 0;

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
    if(DEBUG){
        Serial.print(iter_shade);
        Serial.print(",");
        Serial.print(iter_row);
        Serial.print(",");
        Serial.print(iter_segment);
        Serial.print(", ");
        Serial.print(text);
        Serial.println(value, BIN);
    }
}

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
