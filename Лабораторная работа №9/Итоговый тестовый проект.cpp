```c++
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <IKeypad.h>
#include <ILatch.h>
#include <lockcontroller.h>
using namespace testing;

class MockIKeypad: public IKeypad
{
public:
    MOCK_METHOD(bool, isActive,(),(override));
    MOCK_METHOD(void, wait,(),(override));
    MOCK_METHOD(PasswordResponse, requestPassword,(),(override));
};

class MockILatch:public ILatch
{
public:
    MOCK_METHOD(bool,isActive,(),(override));
    MOCK_METHOD(DoorStatus, open,(),(override));
    MOCK_METHOD(DoorStatus, close,(),(override));
    MOCK_METHOD(DoorStatus,getDoorStatus,(),(override));
};

TEST(Tests, wait)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(keypad,wait())
            .Times(1);// кол-во вызовов
    contrl.wait();
}

TEST(Tests,isDoorOpen_close)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(latch,getDoorStatus())
            .Times(1)
            .WillOnce(Return(DoorStatus::CLOSE));
    bool Return=contrl.isDoorOpen();
    EXPECT_FALSE(Return);
}

TEST(Tests,isDoorClose_open)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(latch,getDoorStatus())
            .Times(1)
            .WillOnce(Return(DoorStatus::OPEN));
    bool Return=contrl.isDoorOpen();
    EXPECT_TRUE(Return);
}

TEST(Tests,unlock_Door)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(latch,open())
            .Times(1);
    DoorStatus Return = contrl.unlockDoor();
    EXPECT_EQ(Return, DoorStatus::OPEN);
}

TEST(Tests, lock_Door)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(latch,close())
            .Times(1)
            .WillOnce(Return(DoorStatus::CLOSE))
    DoorStatus Return = contrl.lockDoor();
    EXPECT_EQ(Return, DoorStatus::OPEN);
}

TEST(Tests,hardWare_Check_OK)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(latch, isActive())
            .Times(1)
            .WillOnce(Return(1));
    EXPECT_CALL(keypad, isActive())
            .Times(1)
            .WillOnce(Return(1));
    HardWareStatus Return = contrl.hardWareCheck();
    EXPECT_EQ(Return,HardWareStatus::OK);
}

TEST(Tests,hardWare_Check_ERROR)
{
    MockIKeypad keypad;
     MockILatch latch;
     LockController contrl = LockController(nullptr,&latch);
     EXPECT_CALL(latch, isActive())
             .Times(AtLeast(0))
             .WillOnce(Return(1));
     EXPECT_CALL(keypad,isActive())
             .Times(AtLeast(0))
             .WillOnce(Return(1));
     HardWareStatus Return = contrl.hardWareCheck();
     EXPECT_EQ(Return,HardWareStatus::ERROR);
}

TEST(Tests, hardWare_Check)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    EXPECT_CALL(latch,isActive())
            .Times(AtLeast(0))
            .WillOnce(Return(0));
    EXPECT_CALL(keypad,isActive)
            .Times(AtLeast(0))
            .WillOnce(Return(1));
    HardWareStatus Return = contrl.hardWareCheck();
    EXPECT_EQ(Return,HardWareStatus::ERROR);
}

TEST(Tests,isCorrect_pass_true)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    PasswordResponse passResp{PasswordResponse::Status::OK,"0000"};
    EXPECT_CALL(keypad, requestPassword())
            .Times(1)
            .WillOnce(Return(passResp));
    bool Return = contrl.isCorrectPassword();
    EXPECT_TRUE(Return);
}

TEST(Tests,isCorrect_pass_false)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    PasswordResponse passResp{PasswordResponse::Status::OK,"1265"};
    EXPECT_CALL(keypad, requestPassword())
            .Times(1)
            .WillOnce(Return(passResp));
    bool Return = contrl.isCorrectPassword();
    EXPECT_FALSE(Return);
}

TEST(Tests, reset_pass)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    PasswordResponse oldPass{PasswordResponse::Status::OK,"0000"};
    PasswordResponse newPass{PasswordResponse::Status::OK,"2503"};
    EXPECT_CALL(keypad, requestPassword())
            .Times(2)
            .WillOnce(Return(oldPass))
            .WillOnce((Return(newPass)));
    contrl.resetPassword();
    EXPECT_CALL(keypad,requestPassword())
            .Times(1)

            .WillOnce(Return(newPass));
    bool Return = contrl.isCorrectPassword();
    EXPECT_TRUE(Return);
}

TEST(Tests, reset_pass2)
{
    MockIKeypad keypad;
    MockILatch latch;
    LockController contrl = LockController(&keypad,&latch);
    PasswordResponse constPass{PasswordResponse::Status::OK,"0000"};
    PasswordResponse oldPass{PasswordResponse::Status::OK,"2503"};
    PasswordResponse newPass{PasswordResponse::Status::OK,"2108"};

    EXPECT_CALL(keypad, requestPassword())
            .Times(2)
            .WillOnce(Return(constPass))
            .WillOnce((Return(oldPass)));
    contrl.resetPassword();

    EXPECT_CALL(keypad,requestPassword())
            .Times(1)
            .WillOnce(Return(oldPass));
    bool returned = contrl.isCorrectPassword();
    EXPECT_TRUE(returned);

    EXPECT_CALL(keypad,requestPassword())
            .Times(2)
            .WillOnce(Return(oldPass))
            .WillOnce(Return(newPass));
    contrl.resetPassword();
    EXPECT_CALL(keypad,requestPassword())
            .Times(1)
            .WillOnce(Return(newPass));
    returned =contrl.isCorrectPassword();
    EXPECT_TRUE(returned);

}
```
