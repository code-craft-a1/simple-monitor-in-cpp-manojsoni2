#include "./monitor.h"
#include <iostream>
#include <thread>
#include <chrono>

// Concrete implementations of vital signs
class TemperatureVital : public VitalSign {
public:
    bool isInRange(float value) const override {
        return value >= 95.0f && value <= 102.0f;
    }
    std::string getName() const override { return "Temperature"; }
    std::string getAlertMessage() const override { return "Temperature is critical!"; }
};

class PulseRateVital : public VitalSign {
public:
    bool isInRange(float value) const override {
        return value >= 60.0f && value <= 100.0f;
    }
    std::string getName() const override { return "Pulse Rate"; }
    std::string getAlertMessage() const override { return "Pulse Rate is out of range!"; }
};

class SpO2Vital : public VitalSign {
public:
    bool isInRange(float value) const override {
        return value >= 90.0f;
    }
    std::string getName() const override { return "SpO2"; }
    std::string getAlertMessage() const override { return "Oxygen Saturation out of range!"; }
};

// Default alert system implementation
class DefaultAlertSystem : public AlertSystem {
public:
    void alert(const std::string& message) override {
        std::cout << message << std::endl;
        for (int i = 0; i < 6; i++) {
            std::cout << "\r* " << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "\r *" << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

// VitalSignMonitor implementation
VitalSignMonitor::VitalSignMonitor() {
    alertSystem = std::make_unique<DefaultAlertSystem>();
    
    // Initialize default vital signs
    vitalSigns.push_back(std::make_shared<TemperatureVital>());
    vitalSigns.push_back(std::make_shared<PulseRateVital>());
    vitalSigns.push_back(std::make_shared<SpO2Vital>());
}

VitalSignMonitor::~VitalSignMonitor() = default;

bool VitalSignMonitor::checkVitals(float temperature, float pulseRate, float spo2) {
    bool allVitalsOk = true;
    
    // Check each vital sign
    for (const auto& vital : vitalSigns) {
        float value;
        if (vital->getName() == "Temperature") {
            value = temperature;
        } else if (vital->getName() == "Pulse Rate") {
            value = pulseRate;
        } else if (vital->getName() == "SpO2") {
            value = spo2;
        } else {
            continue;
        }
        
        if (!vital->isInRange(value)) {
            alertSystem->alert(vital->getAlertMessage());
            allVitalsOk = false;
        }
    }
    
    return allVitalsOk;
}

void VitalSignMonitor::setTemperatureLimits(float min, float max) {
    tempMin = min;
    tempMax = max;
}

void VitalSignMonitor::setPulseRateLimits(float min, float max) {
    pulseMin = min;
    pulseMax = max;
}

void VitalSignMonitor::setSpO2Limits(float min, float max) {
    spo2Min = min;
}

void VitalSignMonitor::setAlertCallback(std::function<void(const std::string&)> callback) {
    class CustomAlertSystem : public AlertSystem {
    public:
        explicit CustomAlertSystem(std::function<void(const std::string&)> cb) : callback(std::move(cb)) {}
        void alert(const std::string& message) override { callback(message); }
    private:
        std::function<void(const std::string&)> callback;
    };
    alertSystem = std::make_unique<CustomAlertSystem>(callback);
}

void VitalSignMonitor::addVitalSign(std::shared_ptr<VitalSign> vital) {
    vitalSigns.push_back(vital);
}

void VitalSignMonitor::removeVitalSign(const std::string& name) {
    vitalSigns.erase(
        std::remove_if(vitalSigns.begin(), vitalSigns.end(),
            [&name](const auto& vital) { return vital->getName() == name; }),
        vitalSigns.end()
    );
}

// Legacy function for backward compatibility
int vitalsOk(float temperature, float pulseRate, float spo2) {
    VitalSignMonitor monitor;
    return monitor.checkVitals(temperature, pulseRate, spo2) ? 1 : 0;
}
