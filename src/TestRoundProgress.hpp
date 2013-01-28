// Default empty project template
#ifndef TestRoundProgress_HPP_
#define TestRoundProgress_HPP_

#include <QObject>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class TestRoundProgress : public QObject
{
    Q_OBJECT
public:
    TestRoundProgress(bb::cascades::Application *app);
    virtual ~TestRoundProgress() {}
};


#endif /* TestRoundProgress_HPP_ */
