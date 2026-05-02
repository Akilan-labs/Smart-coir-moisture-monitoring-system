/*
 * ============================================================
 *  Automated Coir Moisture Monitoring & Control System
 *  PIC16F877A Firmware
 *  Compiler : MikroC Pro for PIC
 * ============================================================
 *
 *  Pin Configuration:
 *  ------------------
 *  RA0/AN0  - Capacitive Moisture Sensor (Analog Input)
 *  RB0      - DHT11 Data Pin
 *  RC2      - Relay Control (Motor ON/OFF)
 *  RD0-RD7  - LCD Data Bus (D0-D7)
 *  RE0      - LCD RS
 *  RE1      - LCD EN
 *  RC6/TX   - UART TX to ESP32
 *  RC7/RX   - UART RX from ESP32
 */

// ---- LCD Pin Definitions ----
sbit LCD_RS at RE0_bit;
sbit LCD_EN at RE1_bit;
sbit LCD_D4 at RD4_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D7 at RD7_bit;

sbit LCD_RS_Direction at TRISE0_bit;
sbit LCD_EN_Direction at TRISE1_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;

// ---- Constants ----
#define RELAY_PIN  RC2_bit
#define MAD_THRESHOLD  40    // Minimum Allowable Depletion (%)
#define FC_THRESHOLD   80    // Field Capacity (%)
#define DRY_ADC        820   // ADC value for completely dry coir (calibrate)
#define WET_ADC        380   // ADC value for fully saturated coir (calibrate)

// ---- Global Variables ----
unsigned int adc_val;
unsigned int moisture_percent;
char lcd_buffer[16];
char uart_buffer[32];

// ---- Function Prototypes ----
unsigned int read_moisture();
void display_data(unsigned int moisture);
void send_to_esp32(unsigned int moisture);

// ============================================================
//  MAIN
// ============================================================
void main() {
    // GPIO Setup
    TRISA  = 0xFF;   // PORTA as input (ADC)
    TRISC2_bit = 0;  // Relay pin as output
    RELAY_PIN = 0;   // Relay off initially

    // ADC Setup
    ADCON1 = 0x80;   // Right justified, Vdd/Vss reference, AN0 as analog

    // UART Setup (9600 baud @ 8MHz)
    UART1_Init(9600);
    Delay_ms(100);

    // LCD Setup
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1, 1, "Coir Monitor");
    Lcd_Out(2, 1, "Initializing...");
    Delay_ms(2000);

    // ---- Main Loop ----
    while(1) {
        moisture_percent = read_moisture();
        display_data(moisture_percent);
        send_to_esp32(moisture_percent);

        // Threshold-based Control Logic
        if (moisture_percent < MAD_THRESHOLD) {
            RELAY_PIN = 1;   // Turn pump ON
        }
        if (moisture_percent >= FC_THRESHOLD) {
            RELAY_PIN = 0;   // Turn pump OFF
        }

        Delay_ms(1000);
    }
}

// ============================================================
//  Read moisture sensor and convert to percentage
//  Uses two-point linear calibration
// ============================================================
unsigned int read_moisture() {
    unsigned int raw;
    unsigned int percent;

    raw = ADC_Read(0);   // Read AN0

    // Clamp to calibration range
    if (raw > DRY_ADC) raw = DRY_ADC;
    if (raw < WET_ADC) raw = WET_ADC;

    // Linear interpolation: higher ADC = drier
    percent = (unsigned int)(((long)(DRY_ADC - raw) * 100) / (DRY_ADC - WET_ADC));

    return percent;
}

// ============================================================
//  Display moisture and pump status on LCD
// ============================================================
void display_data(unsigned int moisture) {
    Lcd_Cmd(_LCD_CLEAR);

    // Line 1: Moisture %
    sprintf(lcd_buffer, "Moisture: %3u%%", moisture);
    Lcd_Out(1, 1, lcd_buffer);

    // Line 2: Pump Status
    if (RELAY_PIN) {
        Lcd_Out(2, 1, "Pump: ON        ");
    } else {
        Lcd_Out(2, 1, "Pump: OFF       ");
    }
}

// ============================================================
//  Send moisture data to ESP32 via UART
//  Format: "M:<value>\n"
// ============================================================
void send_to_esp32(unsigned int moisture) {
    sprintf(uart_buffer, "M:%u\n", moisture);
    UART1_Write_Text(uart_buffer);
}
