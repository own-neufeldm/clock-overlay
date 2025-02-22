#define BUFFER_LENGTH 100

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <time.h>

/**
 * Application state for passing data throughout callbacks without globals.
 */
typedef struct {
  /**
   * Flags to use for this window, such as borderless.
   */
  SDL_WindowFlags windowFlags;

  /**
   * Default geometry to use on startup and when resetting the position. Use
   * zero for width/height to fit window to width/height of text. Use below zero
   * for x/y to place window at width/height of screen minus value.
   */
  SDL_Rect defaultGeometry;

  /**
   * Text color to use.
   */
  SDL_Color foregroundColor;

  /**
   * Window color to use.
   */
  SDL_Color backgroundColor;

  /**
   * Time format to apply as defined by `strftime`, e.g. `%H%M`.
   */
  const char *timeFormat;

  /**
   * A reference value in given format for calculating text size to fit window,
   * e.g. `88:88` for format `%H:%M`.
   */
  const char *timeReference;

  /**
   * Relative path to PNG image file for configuring window icon.
   */
  const char *iconFile;

  /**
   * Relative path to TrueType font file for rendering text.
   */
  const char *fontFile;

  /**
   * Font size, text will be stretched to fit window if need be.
   */
  int fontSize;

  /**
   * Main window.
   */
  SDL_Window *window;

  /**
   * Default renderer.
   */
  SDL_Renderer *renderer;

  /**
   * Default font.
   */
  TTF_Font *font;

  /**
   * Current texture to render, containing `text`.
   */
  SDL_Texture *texture;

  /**
   * Formatted time to draw.
   */
  char text[BUFFER_LENGTH];

  /**
   * Is the user dragging the window?
   */
  bool changePosition;

  /**
   * Geometry to apply when drawing the next frame.
   */
  SDL_Rect requestedGeometry;

  /**
   * Offset start position when window is dragged.
   */
  SDL_Point relativeMousePosition;
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
 * Update the texture containg `text`, but only if it differs.
 *
 * \param state the application state.
 *
 * \returns A boolean value indicating success or failure.
 */
bool updateTexture(AppState *state);
