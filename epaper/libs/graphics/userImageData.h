#pragma once
/*
	userImageData.h
  User set of Image Data
*/

//2.13"
#include <graphics/image_213_212x104_BW.cpp>
#include <graphics/image_213_212x104_BWR.cpp>

#define BW_monoBuffer        (uint8_t *) & image_213_212x104_BW_mono
#define BW_0x00Buffer        (uint8_t *) & image_213_212x104_BW_0x00
#define BWR_blackBuffer      (uint8_t *) & image_213_212x104_BWR_blackBuffer
#define BWR_redBuffer        (uint8_t *) & image_213_212x104_BWR_redBuffer
