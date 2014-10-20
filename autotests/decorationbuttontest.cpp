/*
 * Copyright 2014  Martin Gräßlin <mgraesslin@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QTest>
#include <QSignalSpy>
#include "../src/decoratedclient.h"
#include "mockdecoration.h"
#include "mockbridge.h"
#include "mockbutton.h"
#include "mockclient.h"

Q_DECLARE_METATYPE(Qt::MouseButton)

class DecorationButtonTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testButton();
    void testChecked();
    void testEnabled();
    void testPressIgnore_data();
    void testPressIgnore();
    void testClose();
    void testMinimize();
    void testQuickHelp();
    void testKeepAbove();
    void testKeepBelow();
    void testShade();
    void testMaximize();
};

void DecorationButtonTest::testButton()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    // create a custom button and verify the base settings
    MockButton button(KDecoration2::DecorationButtonType::Custom, &mockDecoration);
    QCOMPARE(button.decoration(), &mockDecoration);
    const MockButton &constRef(button);
    QCOMPARE(constRef.decoration(), &mockDecoration);
    QCOMPARE(button.type(), KDecoration2::DecorationButtonType::Custom);
    QCOMPARE(button.acceptedButtons(), Qt::MouseButtons(Qt::LeftButton));
    QCOMPARE(button.isCheckable(), false);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(button.isHovered(), false);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.size(), QSize(0, 0));
    QCOMPARE(button.geometry(), QRect());

    // test setting the geometry
    QSignalSpy geometryChangedSpy(&button, SIGNAL(geometryChanged(QRect)));
    QVERIFY(geometryChangedSpy.isValid());
    // setting to default geometry shouldn't change
    button.setGeometry(QRect());
    QCOMPARE(button.geometry(), QRect());
    QCOMPARE(geometryChangedSpy.count(), 0);
    // setting to a proper geometry should change
    const QRect testGeometry = QRect(0, 0, 10, 20);
    button.setGeometry(testGeometry);
    QCOMPARE(button.geometry(), testGeometry);
    QCOMPARE(button.size(), testGeometry.size());
    QCOMPARE(geometryChangedSpy.count(), 1);
    QCOMPARE(geometryChangedSpy.first().first().toRect(), testGeometry);

    // test changing visibility
    QSignalSpy visibilityChangedSpy(&button, SIGNAL(visibilityChanged(bool)));
    QVERIFY(visibilityChangedSpy.isValid());
    button.setVisible(true);
    QCOMPARE(visibilityChangedSpy.count(), 0);
    button.setVisible(false);
    QCOMPARE(button.isVisible(), false);
    QCOMPARE(visibilityChangedSpy.count(), 1);
    QCOMPARE(visibilityChangedSpy.first().first().toBool(), false);
}

void DecorationButtonTest::testChecked()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    // create a custom button and verify the base settings
    MockButton button(KDecoration2::DecorationButtonType::Custom, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    // without being checkable it should not get checked
    QSignalSpy checkedChangedSpy(&button, SIGNAL(checkedChanged(bool)));
    QVERIFY(checkedChangedSpy.isValid());
    button.setChecked(true);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(checkedChangedSpy.count(), 0);

    // now let's set the checkable state
    QSignalSpy checkableChangedSpy(&button, SIGNAL(checkableChanged(bool)));
    QVERIFY(checkableChangedSpy.isValid());
    // setting to same should not emit
    button.setCheckable(false);
    QCOMPARE(checkableChangedSpy.count(), 0);
    button.setCheckable(true);
    QCOMPARE(button.isCheckable(), true);
    QCOMPARE(checkableChangedSpy.count(), 1);
    QCOMPARE(checkableChangedSpy.first().first().toBool(), true);

    // now it should be possible to check the button
    button.setChecked(true);
    QCOMPARE(button.isChecked(), true);
    QCOMPARE(checkedChangedSpy.count(), 1);
    // setting again should not change
    button.setChecked(true);
    QCOMPARE(button.isChecked(), true);
    QCOMPARE(checkedChangedSpy.count(), 1);
    // and disable
    button.setChecked(false);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(checkedChangedSpy.count(), 2);
    QCOMPARE(checkedChangedSpy.first().first().toBool(), true);
    QCOMPARE(checkedChangedSpy.last().first().toBool(), false);

    // last but not least let's disable the checkable again, this should disable a checked button
    button.setChecked(true);
    QCOMPARE(button.isChecked(), true);
    checkedChangedSpy.clear();
    QCOMPARE(checkedChangedSpy.count(), 0);
    button.setCheckable(false);
    QCOMPARE(button.isCheckable(), false);
    QCOMPARE(checkableChangedSpy.count(), 2);
    QCOMPARE(checkableChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(checkedChangedSpy.count(), 1);
}

void DecorationButtonTest::testEnabled()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    // create a custom button and verify the base settings
    MockButton button(KDecoration2::DecorationButtonType::Custom, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    // enabling has influence on whether the button accepts events, so we need to fake events
    QSignalSpy enabledChangedSpy(&button, SIGNAL(enabledChanged(bool)));
    QVERIFY(enabledChangedSpy.isValid());
    // setting to same shouldn't change
    button.setEnabled(true);
    QCOMPARE(enabledChangedSpy.count(), 0);
    button.setEnabled(false);
    QCOMPARE(button.isEnabled(), false);
    QCOMPARE(enabledChangedSpy.count(), 1);
    QCOMPARE(enabledChangedSpy.first().first().toBool(), false);

    // now let's send it a hover entered event
    QSignalSpy hoveredChangedSpy(&button, SIGNAL(hoveredChanged(bool)));
    QVERIFY(hoveredChangedSpy.isValid());
    QHoverEvent event(QEvent::HoverEnter, QPointF(1, 1), QPointF(-1, -1));
    event.setAccepted(false);
    button.event(&event);
    QCOMPARE(event.isAccepted(), false);
    QCOMPARE(hoveredChangedSpy.count(), 0);

    // if we enable the button again we should get a hovered changed signal
    button.setEnabled(true);
    QCOMPARE(enabledChangedSpy.count(), 2);
    QCOMPARE(enabledChangedSpy.last().first().toBool(), true);
    button.event(&event);
    QCOMPARE(event.isAccepted(), true);
    QCOMPARE(hoveredChangedSpy.count(), 1);
    QCOMPARE(hoveredChangedSpy.first().first().toBool(), true);

    // if we disable the button now we get a hovered disabled signal
    button.setEnabled(false);
    QCOMPARE(hoveredChangedSpy.count(), 2);
    QCOMPARE(hoveredChangedSpy.last().first().toBool(), false);
}

void DecorationButtonTest::testPressIgnore_data()
{
    QTest::addColumn<bool>("enabled");
    QTest::addColumn<bool>("visible");
    QTest::addColumn<QPoint>("clickPos");
    QTest::addColumn<Qt::MouseButton>("mouseButton");
    QTest::addColumn<bool>("expectedAccepted");

    QTest::newRow("all-disabled") << false << false << QPoint(0, 0) << Qt::LeftButton << false;
    QTest::newRow("enabled") << true << false << QPoint(0, 0) << Qt::LeftButton << false;
    QTest::newRow("visible") << false << true << QPoint(0, 0) << Qt::LeftButton << false;
    QTest::newRow("outside") << true << true << QPoint(20, 20) << Qt::LeftButton << false;
    QTest::newRow("wrong-button") << true << true << QPoint(0, 0) << Qt::RightButton << false;
}

void DecorationButtonTest::testPressIgnore()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    // create a custom button and verify the base settings
    MockButton button(KDecoration2::DecorationButtonType::Custom, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));
    button.setAcceptedButtons(Qt::LeftButton);
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QFETCH(bool, enabled);
    QFETCH(bool, visible);
    button.setEnabled(enabled);
    button.setVisible(visible);

    QFETCH(QPoint, clickPos);
    QFETCH(Qt::MouseButton, mouseButton);
    QMouseEvent pressEvent(QEvent::MouseButtonPress, clickPos, mouseButton, mouseButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QTEST(pressEvent.isAccepted(), "expectedAccepted");
    QCOMPARE(button.isPressed(), false);
    QVERIFY(pressedSpy.isEmpty());
    QVERIFY(pressedChangedSpy.isEmpty());
}

void DecorationButtonTest::testClose()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockClient *client = bridge.lastCreatedClient();
    MockButton button(KDecoration2::DecorationButtonType::Close, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), false);
    QCOMPARE(button.isCheckable(), false);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton);

    // if the client is closeable the button should get enabled
    QSignalSpy closeableChangedSpy(mockDecoration.client(), SIGNAL(closeableChanged(bool)));
    QVERIFY(closeableChangedSpy.isValid());
    client->setCloseable(true);
    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(closeableChangedSpy.count(), 1);
    QCOMPARE(closeableChangedSpy.first().first().toBool(), true);

    // clicking the button should trigger a request for close
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy closeRequestedSpy(client, SIGNAL(closeRequested()));
    QVERIFY(closeRequestedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QCOMPARE(pressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(closeRequestedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    releaseEvent.setAccepted(false);
    button.event(&releaseEvent);
    QCOMPARE(releaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(closeRequestedSpy.wait());
    QCOMPARE(closeRequestedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
}

void DecorationButtonTest::testMinimize()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockClient *client = bridge.lastCreatedClient();
    MockButton button(KDecoration2::DecorationButtonType::Minimize, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), false);
    QCOMPARE(button.isCheckable(), false);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton);

    // if the client is minimizeable the button should get enabled
    QSignalSpy minimizableChangedSpy(mockDecoration.client(), SIGNAL(minimizableChanged(bool)));
    QVERIFY(minimizableChangedSpy.isValid());
    client->setMinimizable(true);
    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(minimizableChangedSpy.count(), 1);
    QCOMPARE(minimizableChangedSpy.first().first().toBool(), true);

    // clicking the button should trigger a request for minimize
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy minimizeRequestedSpy(client, SIGNAL(minimizeRequested()));
    QVERIFY(minimizeRequestedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QCOMPARE(pressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(minimizeRequestedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    releaseEvent.setAccepted(false);
    button.event(&releaseEvent);
    QCOMPARE(releaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(minimizeRequestedSpy.wait());
    QCOMPARE(minimizeRequestedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
}

void DecorationButtonTest::testQuickHelp()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockClient *client = bridge.lastCreatedClient();
    MockButton button(KDecoration2::DecorationButtonType::QuickHelp, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(button.isCheckable(), false);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), false);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton);

    // if the client provides quickhelp the button should get enabled
    QSignalSpy providesContextHelpChangedSpy(mockDecoration.client(), SIGNAL(providesContextHelpChanged(bool)));
    QVERIFY(providesContextHelpChangedSpy.isValid());
    client->setProvidesContextHelp(true);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(providesContextHelpChangedSpy.count(), 1);
    QCOMPARE(providesContextHelpChangedSpy.first().first().toBool(), true);

    // clicking the button should trigger a request for minimize
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy quickhelpRequestedSpy(client, SIGNAL(quickHelpRequested()));
    QVERIFY(quickhelpRequestedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QCOMPARE(pressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(quickhelpRequestedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    releaseEvent.setAccepted(false);
    button.event(&releaseEvent);
    QCOMPARE(releaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(quickhelpRequestedSpy.wait());
    QCOMPARE(quickhelpRequestedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
}

void DecorationButtonTest::testKeepAbove()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockButton button(KDecoration2::DecorationButtonType::KeepAbove, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(button.isCheckable(), true);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton);

    // clicking the button should trigger a request for keep above changed
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy keepAboveChangedSpy(mockDecoration.client(), SIGNAL(keepAboveChanged(bool)));
    QVERIFY(keepAboveChangedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QCOMPARE(pressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(keepAboveChangedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    releaseEvent.setAccepted(false);
    button.event(&releaseEvent);
    QCOMPARE(releaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(keepAboveChangedSpy.wait());
    QCOMPARE(keepAboveChangedSpy.count(), 1);
    QCOMPARE(keepAboveChangedSpy.first().first().toBool(), true);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), true);

    // click once more should change again
    button.event(&pressEvent);
    button.event(&releaseEvent);
    QVERIFY(keepAboveChangedSpy.wait());
    QCOMPARE(keepAboveChangedSpy.count(), 2);
    QCOMPARE(keepAboveChangedSpy.first().first().toBool(), true);
    QCOMPARE(keepAboveChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), false);
}

void DecorationButtonTest::testKeepBelow()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockButton button(KDecoration2::DecorationButtonType::KeepBelow, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(button.isCheckable(), true);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton);

    // clicking the button should trigger a request for keep above changed
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy keepBelowChangedSpy(mockDecoration.client(), SIGNAL(keepBelowChanged(bool)));
    QVERIFY(keepBelowChangedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QCOMPARE(pressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(keepBelowChangedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    releaseEvent.setAccepted(false);
    button.event(&releaseEvent);
    QCOMPARE(releaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(keepBelowChangedSpy.wait());
    QCOMPARE(keepBelowChangedSpy.count(), 1);
    QCOMPARE(keepBelowChangedSpy.first().first().toBool(), true);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), true);

    // click once more should change again
    button.event(&pressEvent);
    button.event(&releaseEvent);
    QVERIFY(keepBelowChangedSpy.wait());
    QCOMPARE(keepBelowChangedSpy.count(), 2);
    QCOMPARE(keepBelowChangedSpy.first().first().toBool(), true);
    QCOMPARE(keepBelowChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), false);
}

void DecorationButtonTest::testShade()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockClient *client = bridge.lastCreatedClient();
    MockButton button(KDecoration2::DecorationButtonType::Shade, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), false);
    QCOMPARE(button.isCheckable(), true);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton);

    // if the client is shadeable the button should get enabled
    QSignalSpy shadeableChangedSpy(mockDecoration.client(), SIGNAL(shadeableChanged(bool)));
    QVERIFY(shadeableChangedSpy.isValid());
    client->setShadeable(true);
    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(shadeableChangedSpy.count(), 1);
    QCOMPARE(shadeableChangedSpy.first().first().toBool(), true);

    // clicking the button should trigger a request for keep above changed
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy shadedChangedSpy(mockDecoration.client(), SIGNAL(shadedChanged(bool)));
    QVERIFY(shadedChangedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    pressEvent.setAccepted(false);
    button.event(&pressEvent);
    QCOMPARE(pressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(shadedChangedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    releaseEvent.setAccepted(false);
    button.event(&releaseEvent);
    QCOMPARE(releaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(shadedChangedSpy.wait());
    QCOMPARE(shadedChangedSpy.count(), 1);
    QCOMPARE(shadedChangedSpy.first().first().toBool(), true);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), true);

    // click once more should change again
    button.event(&pressEvent);
    button.event(&releaseEvent);
    QVERIFY(shadedChangedSpy.wait());
    QCOMPARE(shadedChangedSpy.count(), 2);
    QCOMPARE(shadedChangedSpy.first().first().toBool(), true);
    QCOMPARE(shadedChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), false);
}

void DecorationButtonTest::testMaximize()
{
    MockBridge bridge;
    MockDecoration mockDecoration;
    MockClient *client = bridge.lastCreatedClient();
    MockButton button(KDecoration2::DecorationButtonType::Maximize, &mockDecoration);
    button.setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(button.isEnabled(), false);
    QCOMPARE(button.isCheckable(), true);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(button.isVisible(), true);
    QCOMPARE(button.acceptedButtons(), Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);

    // if the client is maximizable the button should get enabled
    QSignalSpy maximizableChangedSpy(mockDecoration.client(), SIGNAL(maximizableChanged(bool)));
    QVERIFY(maximizableChangedSpy.isValid());
    client->setMaximizable(true);
    QCOMPARE(button.isEnabled(), true);
    QCOMPARE(maximizableChangedSpy.count(), 1);
    QCOMPARE(maximizableChangedSpy.first().first().toBool(), true);

    // clicking the button should trigger a request for keep above changed
    QSignalSpy clickedSpy(&button, SIGNAL(clicked(Qt::MouseButton)));
    QVERIFY(clickedSpy.isValid());
    QSignalSpy pressedSpy(&button, SIGNAL(pressed()));
    QVERIFY(pressedSpy.isValid());
    QSignalSpy releasedSpy(&button, SIGNAL(released()));
    QVERIFY(releasedSpy.isValid());
    QSignalSpy maximizedChangedSpy(mockDecoration.client(), SIGNAL(maximizedChanged(bool)));
    QVERIFY(maximizedChangedSpy.isValid());
    QSignalSpy maximizedVerticallyChangedSpy(mockDecoration.client(), SIGNAL(maximizedVerticallyChanged(bool)));
    QVERIFY(maximizedVerticallyChangedSpy.isValid());
    QSignalSpy maximizedHorizontallyChangedSpy(mockDecoration.client(), SIGNAL(maximizedHorizontallyChanged(bool)));
    QVERIFY(maximizedHorizontallyChangedSpy.isValid());
    QSignalSpy pressedChangedSpy(&button, SIGNAL(pressedChanged(bool)));
    QVERIFY(pressedChangedSpy.isValid());

    QMouseEvent leftPressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    leftPressEvent.setAccepted(false);
    button.event(&leftPressEvent);
    QCOMPARE(leftPressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 0);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 0);
    QCOMPARE(maximizedChangedSpy.count(), 0);
    QCOMPARE(pressedChangedSpy.count(), 1);
    QCOMPARE(pressedChangedSpy.first().first().toBool(), true);

    QMouseEvent leftReleaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    leftReleaseEvent.setAccepted(false);
    button.event(&leftReleaseEvent);
    QCOMPARE(leftReleaseEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedSpy.first().first().value<Qt::MouseButton>(), Qt::LeftButton);
    QEXPECT_FAIL("", "Pressed signal is not yet emitted", Continue);
    QCOMPARE(pressedSpy.count(), 1);
    QEXPECT_FAIL("", "Released signal is not yet emitted", Continue);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(maximizedChangedSpy.wait());
    QCOMPARE(maximizedChangedSpy.count(), 1);
    QCOMPARE(maximizedChangedSpy.first().first().toBool(), true);
    QCOMPARE(pressedChangedSpy.count(), 2);
    QCOMPARE(pressedChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), true);

    // clicking again should set to restored
    button.event(&leftPressEvent);
    button.event(&leftReleaseEvent);
    QVERIFY(maximizedChangedSpy.wait());
    QCOMPARE(maximizedChangedSpy.count(), 2);
    QCOMPARE(maximizedChangedSpy.first().first().toBool(), true);
    QCOMPARE(maximizedChangedSpy.last().first().toBool(), false);
    QCOMPARE(button.isChecked(), false);

    // test the other buttons
    QMouseEvent rightPressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    rightPressEvent.setAccepted(false);
    button.event(&rightPressEvent);
    QCOMPARE(rightPressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);

    QMouseEvent middlePressEvent(QEvent::MouseButtonPress, QPointF(5, 5), Qt::MiddleButton, Qt::MiddleButton, Qt::NoModifier);
    middlePressEvent.setAccepted(false);
    button.event(&middlePressEvent);
    QCOMPARE(middlePressEvent.isAccepted(), true);
    QCOMPARE(button.isPressed(), true);

    QMouseEvent middleReleaseEvent(QEvent::MouseButtonRelease, QPointF(5, 5), Qt::MiddleButton, Qt::NoButton, Qt::NoModifier);
    middleReleaseEvent.setAccepted(false);
    button.event(&middleReleaseEvent);
    QCOMPARE(middleReleaseEvent.isAccepted(), true);
    QVERIFY(maximizedHorizontallyChangedSpy.wait());
    QCOMPARE(button.isPressed(), true);
    QCOMPARE(clickedSpy.count(), 3);
    QCOMPARE(button.isChecked(), false);
    QCOMPARE(client->isMaximizedHorizontally(), true);
    QCOMPARE(client->isMaximizedVertically(), false);

    QMouseEvent rightReleaseEvent(QEvent::QEvent::MouseButtonRelease, QPointF(5, 5), Qt::RightButton, Qt::NoButton, Qt::NoModifier);
    rightReleaseEvent.setAccepted(false);
    button.event(&rightReleaseEvent);
    QCOMPARE(rightReleaseEvent.isAccepted(), true);
    QVERIFY(maximizedVerticallyChangedSpy.wait());
    QCOMPARE(button.isPressed(), false);
    QCOMPARE(clickedSpy.count(), 4);
    QCOMPARE(client->isMaximizedHorizontally(), true);
    QCOMPARE(client->isMaximizedVertically(), true);
    QCOMPARE(button.isChecked(), true);
}

QTEST_MAIN(DecorationButtonTest)
#include "decorationbuttontest.moc"
