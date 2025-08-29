#include <LovyanGFX.hpp>

#include "tft_cfg.h"

#define USE_BACKLIGHT

// Example connection settings for ST7735S (80x160)
#define TFT_MISO -1
#define TFT_MOSI LCD_MOSI_PIN
#define TFT_SCLK LCD_CLK_PIN
#define TFT_CS   LCD_CS_PIN
#define TFT_DC   LCD_DC_PIN
#define TFT_RST  LCD_RST_PIN
#ifdef USE_BACKLIGHT
#define TFT_BLK  LCD_BL_PIN
#endif
#define SPI_PORT  1

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7735S _panel_instance;
  lgfx::Bus_SPI       _bus_instance; // SPI bus instance
#ifdef USE_BACKLIGHT
  lgfx::Light_PWM     _light_instance;
#endif

public:
  LGFX(void)
  {
    {                                    // Configure bus control.
      auto cfg = _bus_instance.config(); // Get the structure for bus configuration.

      // SPI bus settings
      cfg.spi_host = SPI_PORT;   // Select SPI port to use (0 or 1)
      cfg.spi_mode = 0;          // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000; // SPI clock for transmission (max 80MHz, rounded to an integer divisor of 80MHz)
      cfg.freq_read = 20000000;  // SPI clock for reception

      cfg.pin_sclk = TFT_SCLK; // Set SPI SCLK pin number
      cfg.pin_mosi = TFT_MOSI; // Set SPI MOSI pin number
      cfg.pin_miso = TFT_MISO; // Set SPI MISO pin number (-1 = disable)
      cfg.pin_dc   = TFT_DC;   // Set SPI D/C pin number  (-1 = disable)

      _bus_instance.config(cfg);              // Apply settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Set the bus to the panel.
    }

    {                                      // Configure display panel control.
      auto cfg = _panel_instance.config(); // Get the structure for display panel configuration.

      cfg.pin_cs = TFT_CS;   // Pin number where CS is connected   (-1 = disable)
      cfg.pin_rst = TFT_RST; // Pin number where RST is connected  (-1 = disable)
      cfg.pin_busy = -1;     // Pin number where BUSY is connected (-1 = disable)

      cfg.panel_width = 80;   // Actual displayable width
      cfg.panel_height = 160; // Actual displayable height
      cfg.offset_x = 26;      // Panel X-direction offset
      cfg.offset_y = 1;       // Panel Y-direction offset

      // cfg.offset_rotation = 4; // Offset value for rotation direction 0~7 (4~7 are upside down)
      cfg.invert = true; // Set to true if panel brightness is inverted
      // cfg.rgb_order = true;   // Set to true if panel red and blue are swapped

      _panel_instance.config(cfg);
    }

#ifdef USE_BACKLIGHT
    { // Configure backlight control. (Delete if not needed)
      auto cfg = _light_instance.config();    // Get the structure for backlight configuration.

      cfg.pin_bl = TFT_BLK;         // Pin number where backlight is connected
      cfg.invert = false;           // Set to true to invert backlight brightness
      cfg.freq   = 44100;           // PWM frequency for backlight
      cfg.pwm_channel = 6;          // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // Set backlight to the panel.
    }
#endif

    setPanel(&_panel_instance); // Set the panel to use.
  }
};