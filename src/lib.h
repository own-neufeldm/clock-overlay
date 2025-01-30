#define BUFFER_LENGTH 100

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

/**
 * Application state for passing data throughout callbacks.
 *
 * \param windowFlags flags to use for this window, such as borderless.
 * \param defaultGeometry default geometry to use on startup and when resetting
 *                        the position. Use zero for width/height to fit window
 *                        to width/height of text. Use below zero for x/y to
 *                        place window at width/height of screen minus value.
 * \param defaultOpacity default opacity to use on startup and when resetting.
 * \param foregroundColor text color to use.
 * \param backgroundColor window color to use.
 * \param timeFormat time format to apply as defined by `strftime`, e.g. `%H%M`.
 * \param timeReference a reference value in given format for calculating text
 *                      size to fit window, e.g. `88:88` for format `%H:%M`.
 * \param fontFile relative path to True-Type-Font file for rendering text.
 * \param fontSize font size, text will be stretched to fit window if need be.
 *
 */
typedef struct {
  // style configured by developer
  SDL_WindowFlags windowFlags;
  SDL_Rect defaultGeometry;
  float defaultOpacity;
  SDL_Color foregroundColor;
  SDL_Color backgroundColor;
  const char *timeFormat;
  const char *timeReference;
  const char *fontFile;
  int fontSize;

  // resources managed by application
  SDL_Window *window;
  SDL_Renderer *renderer;
  TTF_Font *font;
  SDL_Texture *texture;
  char text[BUFFER_LENGTH];
  bool changePosition;
  SDL_Rect requestedGeometry;
  SDL_Point relativeMousePosition;
  bool changeOpacity;
  float requestedOpacity;
} AppState;

/**
 * Loads the font used for rendering text.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadFont(AppState *state);

/**
 * Loads the main window of this application. Requires font to be loaded.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadWindow(AppState *state);

/**
 * Loads the default renderer. Requires window to be loaded.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool loadRenderer(AppState *state);

/**
 * Update window position to requested value, but only if it differs.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updatePosition(AppState *state);

/**
 * Update window opacity to requested value, but only if it differs.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updateOpacity(AppState *state);

/**
 * Render to the buffer, creating a new texture first if text has changed.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool renderTexture(AppState *state);
