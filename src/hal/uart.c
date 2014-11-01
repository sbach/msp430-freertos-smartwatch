#include <msp430.h>
#include "uart.h"


/*******************************************************************************
 * \brief   Setup UCA1 UART interface
 *
 * \param void
 * \return void
 ******************************************************************************/
void halDebugUARTInit( void )
{
    // Set P5.4 and P5.6 to UCA function
    P5SEL |= BIT5;
    P5SEL |= BIT6;

    // Put the USCI state machine in reset
    UCA1CTL1 = UCSWRST;

    // Select the clock source
    UCA1CTL1 |= UCSSEL__ACLK;


    // Configure the baud rate
    // @see page 905, 906 of slau208n.pdf

    // CLK          baud    BRx     BRSx    BRFx
    // -----------------------------------------
    // 32768 Hz     9600    3       0       3

    // Prescaler value UCBRx (16-bit)
    // @see page 913 of slau208n.pdf
    // BRx = (BR0 + BR1 × 256)
    UCA1BR0 = 0x03;
    UCA1BR1 = 0x00;

    // First and second modulation stage selection
    // These bits determine the modulation pattern.
    // @see page 913 of slau208n.pdf
    UCA1MCTL = UCBRS_3 + UCBRF_0;


    // Reset UCAxSTAT register to clear error flags
    UCA1STAT = 0;

    // Initialize USCI state machine
    UCA1CTL1 &= ~UCSWRST;

    // Enable USCI_A1 RX interrupt
    UCA1IE = UCRXIE;

    // Clear interrupt flags
    UCA1IFG = 0;
}

/*******************************************************************************
 * \brief   Write a string to the UART TX buffer
 *
 * \param const char *  Text to write
 * \return void
 ******************************************************************************/
void halDebugUARTWrite(const char *buf)
{
    // Send each character to the TX buffer
    while (*buf) WRITE_DEBUG_UART(*buf++);

    // Wait until transmit is done
    while (UCA1STAT & UCBUSY);
}

/*******************************************************************************
 * \brief   ISR to handle events on the USCI_A1 pins.
 *
 * \param void
 * \return void
 ******************************************************************************/
void __attribute__ ( ( interrupt(USCI_A1_VECTOR) ) ) halDebugUARTISR( void )
{
    switch (__even_in_range(UCA1IV,4)) {
        case UART_NO_INTERRUPT:
            break;
        case UART_RX_IFG:
            UCA1IFG  &= ~UCRXIFG;
            UCA1TXBUF = UCA1RXBUF;
            break;
        case UART_TX_IFG:
            break;
        default:
            break;
    }
}
