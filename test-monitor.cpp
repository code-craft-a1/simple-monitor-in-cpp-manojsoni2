#include <gtest/gtest.h>
#include "./monitor.h"
#include <string>
#include <vector>

// Test fixture for VitalSignMonitor
class VitalSignMonitorTest : public ::testing::Test {
protected:
    VitalSignMonitor monitor;
    std::vector<std::string> alertMessages;
    
    void SetUp() override {
        // Set up custom alert callback to capture messages
        monitor.setAlertCallback([this](const std::string& message) {
            alertMessages.push_back(message);
        });
    }
    
    void TearDown() override {
        alertMessages.clear();
    }
};

// Test basic vital sign ranges
TEST_F(VitalSignMonitorTest, AllVitalsInRange) {
    EXPECT_TRUE(monitor.checkVitals(98.6f, 75.0f, 95.0f));
    EXPECT_TRUE(alertMessages.empty());
}

TEST_F(VitalSignMonitorTest, TemperatureOutOfRange) {
    EXPECT_FALSE(monitor.checkVitals(103.0f, 75.0f, 95.0f));
    EXPECT_EQ(alertMessages.size(), 1);
    EXPECT_EQ(alertMessages[0], "Temperature is critical!");
}

TEST_F(VitalSignMonitorTest, PulseRateOutOfRange) {
    EXPECT_FALSE(monitor.checkVitals(98.6f, 110.0f, 95.0f));
    EXPECT_EQ(alertMessages.size(), 1);
    EXPECT_EQ(alertMessages[0], "Pulse Rate is out of range!");
}

TEST_F(VitalSignMonitorTest, SpO2OutOfRange) {
    EXPECT_FALSE(monitor.checkVitals(98.6f, 75.0f, 85.0f));
    EXPECT_EQ(alertMessages.size(), 1);
    EXPECT_EQ(alertMessages[0], "Oxygen Saturation out of range!");
}

TEST_F(VitalSignMonitorTest, MultipleVitalsOutOfRange) {
    EXPECT_FALSE(monitor.checkVitals(103.0f, 110.0f, 85.0f));
    EXPECT_EQ(alertMessages.size(), 3);
}

// Test custom limits
TEST_F(VitalSignMonitorTest, CustomTemperatureLimits) {
    monitor.setTemperatureLimits(96.0f, 100.0f);
    EXPECT_FALSE(monitor.checkVitals(95.0f, 75.0f, 95.0f));
    EXPECT_EQ(alertMessages.size(), 1);
    EXPECT_EQ(alertMessages[0], "Temperature is critical!");
}

TEST_F(VitalSignMonitorTest, CustomPulseRateLimits) {
    monitor.setPulseRateLimits(70.0f, 90.0f);
    EXPECT_FALSE(monitor.checkVitals(98.6f, 95.0f, 95.0f));
    EXPECT_EQ(alertMessages.size(), 1);
    EXPECT_EQ(alertMessages[0], "Pulse Rate is out of range!");
}

TEST_F(VitalSignMonitorTest, CustomSpO2Limits) {
    monitor.setSpO2Limits(95.0f, 100.0f);
    EXPECT_FALSE(monitor.checkVitals(98.6f, 75.0f, 94.0f));
    EXPECT_EQ(alertMessages.size(), 1);
    EXPECT_EQ(alertMessages[0], "Oxygen Saturation out of range!");
}

// Test legacy function
TEST(Monitor, LegacyFunction) {
    EXPECT_FALSE(vitalsOk(103.0f, 75.0f, 95.0f));
    EXPECT_TRUE(vitalsOk(98.6f, 75.0f, 95.0f));
}

// Test boundary conditions
TEST_F(VitalSignMonitorTest, BoundaryConditions) {
    // Temperature boundaries
    EXPECT_TRUE(monitor.checkVitals(95.0f, 75.0f, 95.0f));  // Min temp
    EXPECT_TRUE(monitor.checkVitals(102.0f, 75.0f, 95.0f)); // Max temp
    
    // Pulse rate boundaries
    EXPECT_TRUE(monitor.checkVitals(98.6f, 60.0f, 95.0f));  // Min pulse
    EXPECT_TRUE(monitor.checkVitals(98.6f, 100.0f, 95.0f)); // Max pulse
    
    // SpO2 boundary
    EXPECT_TRUE(monitor.checkVitals(98.6f, 75.0f, 90.0f));  // Min SpO2
}
