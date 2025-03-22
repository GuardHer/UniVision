#pragma once

#include <string>

struct UniSaveConfig
{
	bool _isSvaeOriginalImage = false;  // Save original image
	bool _isSaveResultImage = false;    // Save result image

	std::string _saveOriginalImageDir = "";  // Original image save directory
	std::string _saveResultImageDir = "";    // Result image save directory

};

