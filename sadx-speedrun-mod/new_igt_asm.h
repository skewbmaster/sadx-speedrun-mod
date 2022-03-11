#pragma once

static void* sonicAddressPointers[] = { (void*)0x77ED40, (void*)0x42F04B, (void*)0x42F064, (void*)0x4380C2, (void*)0x40CC69 };

static void beforeMoviePlay();

static void afterMoviePlay();

static void incrementFromCameraMain();
static void incrementFromMainFrameCounter();

static void checkCredits();