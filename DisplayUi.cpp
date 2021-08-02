/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
 * Copyright (c) 2018 by Fabrice Weinberg
 * Copyright (c) 2019 by Helmut Tschemernjak - www.radioshuttle.de
 * Copyright (c) 2021 by Jack Burgess
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ThingPulse invests considerable time and money to develop these open source
 * libraries. Please support us by buying our products (and not the clones) from
 * https://thingpulse.com
 *
 */

#include "DisplayUi.h"

void LoadingDrawDefault(TFT_eSPI *tft, LoadingStage *stage, uint8_t progress) {
  tft->setTextDatum(MC_DATUM);
  tft->drawString(stage->process, tft->width() / 2, tft->height() / 3, 4);
  tft->setTextDatum(TL_DATUM);
  // Progress bar
  int x = tft->width() / 4;
  int y = tft->height() / 2;
  int w = tft->width() / 2;
  int h = 16;
  // Progress bar outline
  tft->drawRoundRect(x, y, w, h, 4, TFT_WHITE);
  // Filled portion
  tft->fillRoundRect(x, y, ((w / 100.0) * progress) + 2, h, 4, TFT_WHITE);
};

DisplayUi::DisplayUi(TFT_eSPI *tft) {
  this->tft = tft;

  indicatorPosition = BOTTOM;
  indicatorDirection = LEFT_RIGHT;
  activeSymbol = ANIMATION_activeSymbol;
  inactiveSymbol = ANIMATION_inactiveSymbol;
  frameAnimationDirection = SLIDE_RIGHT;
  lastTransitionDirection = 1;
  frameCount = 0;
  nextFrameNumber = -1;
  overlayCount = 0;
  indicatorDrawState = 1;
  loadingDrawFunction = LoadingDrawDefault;
  updateInterval = 33;
  state.lastUpdate = 0;
  state.ticksSinceLastStateSwitch = 0;
  state.frameState = FIXED;
  state.currentFrame = 0;
  state.frameTransitionDirection = 1;
  state.isIndicatorDrawn = true;
  state.manualControl = false;
  state.userData = NULL;
  shouldDrawIndicators = true;
  autoTransition = true;
  setTimePerFrame(5000);
  setTimePerTransition(500);
}

void DisplayUi::init() {
  // this->tft->init();
}

void DisplayUi::setTargetFPS(uint8_t fps) {
  this->updateInterval = ((float)1.0 / (float)fps) * 1000;

  this->ticksPerFrame = timePerFrame / updateInterval;
  this->ticksPerTransition = timePerTransition / updateInterval;
}

// -/------ Automatic control ------\-

void DisplayUi::enableAutoTransition() { this->autoTransition = true; }
void DisplayUi::disableAutoTransition() { this->autoTransition = false; }
void DisplayUi::setAutoTransitionForwards() {
  this->state.frameTransitionDirection = 1;
  this->lastTransitionDirection = 1;
}
void DisplayUi::setAutoTransitionBackwards() {
  this->state.frameTransitionDirection = -1;
  this->lastTransitionDirection = -1;
}
void DisplayUi::setTimePerFrame(uint16_t time) {
  this->timePerFrame = time;
  this->ticksPerFrame = timePerFrame / updateInterval;
}
void DisplayUi::setTimePerTransition(uint16_t time) {
  this->timePerTransition = time;
  this->ticksPerTransition = timePerTransition / updateInterval;
}

// -/------ Customize indicator position and style -------\-
void DisplayUi::enableIndicator() { this->state.isIndicatorDrawn = true; }

void DisplayUi::disableIndicator() { this->state.isIndicatorDrawn = false; }

void DisplayUi::enableAllIndicators() { this->shouldDrawIndicators = true; }

void DisplayUi::disableAllIndicators() { this->shouldDrawIndicators = false; }

void DisplayUi::setIndicatorPosition(IndicatorPosition pos) {
  this->indicatorPosition = pos;
}
void DisplayUi::setIndicatorDirection(IndicatorDirection dir) {
  this->indicatorDirection = dir;
}
void DisplayUi::setActiveSymbol(const uint8_t *symbol) {
  this->activeSymbol = symbol;
}
void DisplayUi::setInactiveSymbol(const uint8_t *symbol) {
  this->inactiveSymbol = symbol;
}

// -/----- Frame settings -----\-
void DisplayUi::setFrameAnimation(AnimationDirection dir) {
  this->frameAnimationDirection = dir;
}
void DisplayUi::setFrames(FrameCallback *frameFunctions, uint8_t frameCount) {
  this->frameFunctions = frameFunctions;
  this->frameCount = frameCount;
  this->resetState();
}

// -/----- Overlays ------\-
void DisplayUi::setOverlays(OverlayCallback *overlayFunctions,
                            uint8_t overlayCount) {
  this->overlayFunctions = overlayFunctions;
  this->overlayCount = overlayCount;
}

// -/----- Loading Process -----\-

void DisplayUi::setLoadingDrawFunction(
    LoadingDrawFunction loadingDrawFunction) {
  this->loadingDrawFunction = loadingDrawFunction;
}

void DisplayUi::runLoadingProcess(LoadingStage *stages, uint8_t stagesCount) {
  uint8_t progress = 0;
  uint8_t increment = 100 / (stagesCount - 1);

  for (uint8_t i = 0; i < stagesCount; i++) {
    // tft->fillScreen(TFT_BLACK);
    this->loadingDrawFunction(this->tft, &stages[i], progress);
    // display->display();

    stages[i].callback();

    progress += increment;
    yield();
  }

  tft->fillScreen(TFT_BLACK);

  delay(150);
}

// -/----- Manual control -----\-
void DisplayUi::nextFrame() {
  if (this->state.frameState != IN_TRANSITION) {
    this->state.manualControl = true;
    this->state.frameState = IN_TRANSITION;
    this->state.ticksSinceLastStateSwitch = 0;
    this->lastTransitionDirection = this->state.frameTransitionDirection;
    this->state.frameTransitionDirection = 1;
  }
}
void DisplayUi::previousFrame() {
  if (this->state.frameState != IN_TRANSITION) {
    this->state.manualControl = true;
    this->state.frameState = IN_TRANSITION;
    this->state.ticksSinceLastStateSwitch = 0;
    this->lastTransitionDirection = this->state.frameTransitionDirection;
    this->state.frameTransitionDirection = -1;
  }
}

void DisplayUi::switchToFrame(uint8_t frame) {
  if (frame >= this->frameCount)
    return;
  this->state.ticksSinceLastStateSwitch = 0;
  if (frame == this->state.currentFrame)
    return;
  this->state.frameState = FIXED;
  this->state.currentFrame = frame;
  this->state.isIndicatorDrawn = true;
}

void DisplayUi::transitionToFrame(uint8_t frame) {
  if (frame >= this->frameCount)
    return;
  this->state.ticksSinceLastStateSwitch = 0;
  if (frame == this->state.currentFrame)
    return;
  this->nextFrameNumber = frame;
  this->lastTransitionDirection = this->state.frameTransitionDirection;
  this->state.manualControl = true;
  this->state.frameState = IN_TRANSITION;
  this->state.frameTransitionDirection =
      frame < this->state.currentFrame ? -1 : 1;
}

// -/----- State information -----\-
DisplayUiState *DisplayUi::getUiState() { return &this->state; }

int16_t DisplayUi::update() {
#ifdef ARDUINO
  unsigned long frameStart = millis();
#else
#error "Unkown operating system"
#endif
  int32_t timeBudget =
      this->updateInterval - (frameStart - this->state.lastUpdate);
  if (timeBudget <= 0) {
    // Implement frame skipping to ensure time budget is kept
    if (this->autoTransition && this->state.lastUpdate != 0) {
      this->state.ticksSinceLastStateSwitch +=
          ceil((double)-timeBudget / (double)this->updateInterval);
    }

    this->state.lastUpdate = frameStart;
    this->tick();
  }
#ifdef ARDUINO
  return this->updateInterval - (millis() - frameStart);
#else
#error "Unknown operating system"
#endif
}

void DisplayUi::tick() {
  this->state.ticksSinceLastStateSwitch++;

  switch (this->state.frameState) {
  case IN_TRANSITION:
    if (this->state.ticksSinceLastStateSwitch >= this->ticksPerTransition) {
      this->state.frameState = FIXED;
      this->state.currentFrame = getNextFrameNumber();
      this->state.ticksSinceLastStateSwitch = 0;
      this->nextFrameNumber = -1;
    }
    break;
  case FIXED:
    // Revert manualControl
    if (this->state.manualControl) {
      this->state.frameTransitionDirection = this->lastTransitionDirection;
      this->state.manualControl = false;
    }
    if (this->state.ticksSinceLastStateSwitch >= this->ticksPerFrame) {
      if (this->autoTransition) {
        this->state.frameState = IN_TRANSITION;
      }
      this->state.ticksSinceLastStateSwitch = 0;
    }
    break;
  }

  this->tft->fillScreen(TFT_BLACK);
  this->drawFrame();
  if (shouldDrawIndicators) {
    this->drawIndicator();
  }
  this->drawOverlays();
}

void DisplayUi::resetState() {
  this->state.lastUpdate = 0;
  this->state.ticksSinceLastStateSwitch = 0;
  this->state.frameState = FIXED;
  this->state.currentFrame = 0;
  this->state.isIndicatorDrawn = true;
}

void DisplayUi::drawFrame() {
  switch (this->state.frameState) {
  case IN_TRANSITION: {
    float progress = 0.f;
    if (this->ticksPerTransition > 0u) {
      progress = (float)this->state.ticksSinceLastStateSwitch /
                 (float)this->ticksPerTransition;
    }
    int16_t x = 0, y = 0, x1 = 0, y1 = 0;
    switch (this->frameAnimationDirection) {
    case SLIDE_LEFT:
      x = -this->tft->width() * progress;
      y = 0;
      x1 = x + this->tft->width();
      y1 = 0;
      break;
    case SLIDE_RIGHT:
      x = this->tft->width() * progress;
      y = 0;
      x1 = x - this->tft->width();
      y1 = 0;
      break;
    case SLIDE_UP:
      x = 0;
      y = -this->tft->height() * progress;
      x1 = 0;
      y1 = y + this->tft->height();
      break;
    case SLIDE_DOWN:
    default:
      x = 0;
      y = this->tft->height() * progress;
      x1 = 0;
      y1 = y - this->tft->height();
      break;
    }

    // Invert animation if direction is reversed.
    int8_t dir = this->state.frameTransitionDirection >= 0 ? 1 : -1;
    x *= dir;
    y *= dir;
    x1 *= dir;
    y1 *= dir;

    bool drawnCurrentFrame;

    // Probe each frameFunction for the indicator drawn state
    this->enableIndicator();
    (this->frameFunctions[this->state.currentFrame])(this->tft, &this->state, x,
                                                     y);
    drawnCurrentFrame = this->state.isIndicatorDrawn;

    this->enableIndicator();
    (this->frameFunctions[this->getNextFrameNumber()])(this->tft, &this->state,
                                                       x1, y1);

    // Build up the indicatorDrawState
    if (drawnCurrentFrame && !this->state.isIndicatorDrawn) {
      // Drawn now but not next
      this->indicatorDrawState = 2;
    } else if (!drawnCurrentFrame && this->state.isIndicatorDrawn) {
      // Not drawn now but next
      this->indicatorDrawState = 1;
    } else if (!drawnCurrentFrame && !this->state.isIndicatorDrawn) {
      // Not drawn in both frames
      this->indicatorDrawState = 3;
    }

    // If the indicator isn't drawn in the current frame
    // reflect it in state.isIndicatorDrawn
    if (!drawnCurrentFrame)
      this->state.isIndicatorDrawn = false;

    break;
  }
  case FIXED:
    // Always assume that the indicator is drawn!
    // And set indicatorDrawState to "not known yet"
    this->indicatorDrawState = 0;
    this->enableIndicator();
    (this->frameFunctions[this->state.currentFrame])(this->tft, &this->state, 0,
                                                     0);
    break;
  }
}

void DisplayUi::drawIndicator() {

  // Only draw if the indicator is invisible
  // for both frames or
  // the indiactor is shown and we are IN_TRANSITION
  if (this->indicatorDrawState == 3 ||
      (!this->state.isIndicatorDrawn &&
       this->state.frameState != IN_TRANSITION)) {
    return;
  }

  uint8_t posOfHighlightFrame = 0;
  float indicatorFadeProgress = 0;

  // if the indicator needs to be slided in we want to
  // highlight the next frame in the transition
  uint8_t frameToHighlight = this->indicatorDrawState == 1
                                 ? this->getNextFrameNumber()
                                 : this->state.currentFrame;

  // Calculate the frame that needs to be highlighted
  // based on the Direction the indiactor is drawn
  switch (this->indicatorDirection) {
  case LEFT_RIGHT:
    posOfHighlightFrame = frameToHighlight;
    break;
  case RIGHT_LEFT:
  default:
    posOfHighlightFrame = this->frameCount - frameToHighlight;
    break;
  }

  switch (this->indicatorDrawState) {
  case 1: // Indicator was not drawn in this frame but will be in next
    // Slide IN
    indicatorFadeProgress = 1 - ((float)this->state.ticksSinceLastStateSwitch /
                                 (float)this->ticksPerTransition);
    break;
  case 2: // Indicator was drawn in this frame but not in next
    // Slide OUT
    indicatorFadeProgress = ((float)this->state.ticksSinceLastStateSwitch /
                             (float)this->ticksPerTransition);
    break;
  }

  // Space between indicators - reduce for small screen sizes
  uint16_t indicatorSpacing = 12;
  if (this->tft->height() < 64 &&
      (this->indicatorPosition == RIGHT || this->indicatorPosition == LEFT)) {
    indicatorSpacing = 6;
  }

  uint16_t frameStartPos = (indicatorSpacing * frameCount / 2);
  const uint8_t *image;

  uint16_t x = 0, y = 0;

  for (uint8_t i = 0; i < this->frameCount; i++) {

    switch (this->indicatorPosition) {
    case TOP:
      y = 0 - (8 * indicatorFadeProgress);
      x = (this->tft->width() / 2) - frameStartPos + 12 * i;
      break;
    case BOTTOM:
      y = (this->tft->height() - 8) + (8 * indicatorFadeProgress);
      x = (this->tft->width() / 2) - frameStartPos + 12 * i;
      break;
    case RIGHT:
      x = (this->tft->width() - 8) + (8 * indicatorFadeProgress);
      y = (this->tft->height() / 2) - frameStartPos + 2 + 12 * i;
      break;
    case LEFT:
    default:
      x = 0 - (8 * indicatorFadeProgress);
      y = (this->tft->height() / 2) - frameStartPos + 2 + indicatorSpacing * i;
      break;
    }

    if (posOfHighlightFrame == i) {
      image = this->activeSymbol;
    } else {
      image = this->inactiveSymbol;
    }

    this->tft->pushImage(x, y, 8, 8, (uint8_t *)image);
  }
}

void DisplayUi::drawOverlays() {
  for (uint8_t i = 0; i < this->overlayCount; i++) {
    (this->overlayFunctions[i])(this->tft, &this->state);
  }
}

uint8_t DisplayUi::getNextFrameNumber() {
  if (this->nextFrameNumber != -1) {
    return this->nextFrameNumber;
  }
  return (this->state.currentFrame + this->frameCount +
          this->state.frameTransitionDirection) %
         this->frameCount;
}
