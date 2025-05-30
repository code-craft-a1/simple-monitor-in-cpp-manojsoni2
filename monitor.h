#pragma once
#include <string>
#include <functional>
#include <vector>
#include <memory>

// Forward declarations
class VitalSign;
class AlertSystem;

class VitalSignMonitor {
public:
    VitalSignMonitor();
    ~VitalSignMonitor();

    // Core monitoring functionality
    bool checkVitals(float temperature, float pulseRate, float spo2);
    
    // Configuration methods
    void setTemperatureLimits(float min, float max);
    void setPulseRateLimits(float min, float max);
    void setSpO2Limits(float min, float max);
    
    // Alert system configuration
    void setAlertCallback(std::function<void(const std::string&)> callback);
    
    // Future extensibility
    void addVitalSign(std::shared_ptr<VitalSign> vital);
    void removeVitalSign(const std::string& name);

private:
    std::vector<std::shared_ptr<VitalSign>> vitalSigns;
    std::unique_ptr<AlertSystem> alertSystem;
    
    // Thresholds
    float tempMin = 95.0f;
    float tempMax = 102.0f;
    float pulseMin = 60.0f;
    float pulseMax = 100.0f;
    float spo2Min = 90.0f;
};

// Base class for vital signs
class VitalSign {
public:
    virtual ~VitalSign() = default;
    virtual bool isInRange(float value) const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getAlertMessage() const = 0;
};

// Alert system interface
class AlertSystem {
public:
    virtual ~AlertSystem() = default;
    virtual void alert(const std::string& message) = 0;
};
