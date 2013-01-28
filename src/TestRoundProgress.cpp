// Default empty project template
#include "TestRoundProgress.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "RoundProgress.hpp"

using namespace bb::cascades;

TestRoundProgress::TestRoundProgress(bb::cascades::Application *app)
: QObject(app)
{
    qmlRegisterType<RoundProgress>("TestRoundProgress", 1, 0, "RoundProgress");

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // set created root object as a scene
    app->setScene(root);
}

