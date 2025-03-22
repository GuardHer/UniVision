#include "UniBaseOnnxDetect.h"

#include "UniOnnxModel.h"

UniBaseOnnxDetect::UniBaseOnnxDetect(const AlgotithmParam &param) : _param(param)

{
}

bool UniBaseOnnxDetect::init(const std::string &modelPath, bool isCuda, int cudaID, bool warmUp)
{
    if (_model) {
        return true;
	}

    _model = std::make_unique<UniOnnxModel>(modelPath);
    if (!_model->loadModel(isCuda, cudaID, warmUp)) {
        return false;
    }

    return true;
}
