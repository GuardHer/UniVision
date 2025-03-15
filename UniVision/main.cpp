#include "UniVision.h"
#include <QtWidgets/QApplication>
#include <ElaApplication.h>

#include "test/Test.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
    //根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif

	//TestAll();
    //TestAlgorithm(argc, argv);
    QApplication a(argc, argv);
    eApp->init();

    UniVision w;
    w.show();
    return a.exec();
}
