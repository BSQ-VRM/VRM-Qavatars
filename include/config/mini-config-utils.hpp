#pragma once
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "rapidjson-macros/shared/macros.hpp"

#define DECLARE_CONFIG(name, ...) \
DECLARE_JSON_CLASS(name##_t, \
    DISCARD_EXTRA_FIELDS \
    __VA_ARGS__ \
    private: \
    static inline std::string __config_path = ""; \
    static inline name##_t* __self_instance = nullptr; \
    public: \
    static void Init(std::string path) { \
        __config_path = path; \
        if(!__self_instance) \
            __self_instance = new name##_t(); \
        try { \
            ReadFromFile(__config_path, GetInstance()); \
        } catch (const std::exception& err) { \
            ConfigUtils::getLogger().error("Error reading config: %s (from %s)", err.what(), __config_path.c_str()); \
        } \
		Save(); \
    } \
    static inline name##_t& GetInstance() { \
        return *__self_instance; \
    } \
    static void Save() { \
        if (__config_path.empty()) { \
            ConfigUtils::getLogger().error("Config was not initialized!"); \
            return; \
        } \
        try { \
            WriteToFile(__config_path, GetInstance()); \
        } catch (const std::exception& err) { \
            ConfigUtils::getLogger().error("Error saving config: %s (to %s)", err.what(), __config_path.c_str()); \
        } \
    } \
) \
inline name##_t& get##name() { \
    return name##_t::GetInstance(); \
}

#define CONFIG_VALUE(name, type, jsonName, def, ...) \
private: \
NAMED_VALUE_DEFAULT(ConfigUtils::Specialization<type>::JSONType, __##name, def, jsonName) \
public: \
ConfigUtils::ConfigValue<type> name = {&this->Save, &__##name, jsonName, def __VA_OPT__(,) __VA_ARGS__};

namespace ConfigUtils {

    inline Logger& getLogger() {
        static auto logger = new Logger(ModInfo{"config-utils", "1.1.0"});
        return *logger;
    }

    template <typename ValueType>
    struct Specialization {
        using JSONType = ValueType;
    };

    template <typename ValueType>
    class ConfigValue {
        private:
            using JSONType = typename ConfigUtils::Specialization<ValueType>::JSONType;
            std::string name;
            JSONType* value;
            ValueType defaultValue;
            std::string hoverHint;
            void(*saveFunc)();
            std::vector<std::function<void(ValueType)>> changeEvents;
            std::mutex changeEventsMutex;

        public:
            bool operator==(const ConfigValue<ValueType>& other) const {
                return *value == other.GetValue()
                    && defaultValue == other.GetDefaultValue()
                    && name == other.GetName()
                    && hoverHint == other.GetHoverHint();
            }

            ConfigValue(void(*save)(), JSONType* ref, std::string name, ValueType defaultValue) {
                this->saveFunc = save;
                this->value = ref;
                this->name = name;
                this->defaultValue = defaultValue;
            }

            ConfigValue(void(*save)(), JSONType* ref, std::string name, ValueType defaultValue, std::string hoverHint) : ConfigValue(save, ref, name, defaultValue) {
                this->hoverHint = hoverHint;
            }

            void SaveValue() {
                saveFunc();
            }

            ValueType GetValue() {
                return (ValueType) *value;
            }

            void SetValue(ValueType value, bool save = true) {
                *this->value = value;
                if(save)
                    SaveValue();
                std::lock_guard<std::mutex> lock(changeEventsMutex);
                for (auto& event : changeEvents) {
                    event(value);
                }
            }

            ValueType GetDefaultValue() {
                return defaultValue;
            }

            std::string GetName() {
                return name;
            }

            std::string GetHoverHint() {
                return hoverHint;
            }

            void AddChangeEvent(std::function<void(ValueType)> event) {
                std::lock_guard<std::mutex> lock(changeEventsMutex);
                changeEvents.push_back(event);
            }
    };
}

#pragma region CodegenValues
#if __has_include("UnityEngine/Vector2.hpp")

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Color.hpp"

namespace ConfigUtils {

#pragma region JSONClasses

#define CONVERSION(clazz, construct, convert) \
clazz(const UnityEngine::clazz& other) { construct; } \
clazz& operator=(const UnityEngine::clazz& other) { construct; return *this; } \
operator UnityEngine::clazz() const { return UnityEngine::clazz convert; }

DECLARE_JSON_CLASS(Vector2,
    VALUE(float, x)
    VALUE(float, y)
    CONVERSION(Vector2,
        x = other.x; y = other.y;,
        (x, y)
    )
    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}
)

DECLARE_JSON_CLASS(Vector3,
    VALUE(float, x)
    VALUE(float, y)
    VALUE(float, z)
    CONVERSION(Vector3,
        x = other.x; y = other.y; z = other.z;,
        (x, y, z)
    )
    Vector3() = default;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
)

DECLARE_JSON_CLASS(Vector4,
    VALUE(float, x)
    VALUE(float, y)
    VALUE(float, z)
    VALUE(float, w)
    CONVERSION(Vector4,
        x = other.x; y = other.y; z = other.z; w = other.w;,
        (x, y, z, w)
    )
    Vector4() = default;
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
)

DECLARE_JSON_CLASS(Color,
    VALUE(float, r)
    VALUE(float, g)
    VALUE(float, b)
    VALUE(float, a)
    CONVERSION(Color,
        r = other.r; g = other.g; b = other.b; a = other.a;,
        (r, g, b, a)
    )
    Color() = default;
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
)

#pragma endregion

#pragma region Specializations

#define SPECIALIZATION(type) \
template <> \
struct Specialization<UnityEngine::type> { \
    using JSONType = type; \
}; \
template <> \
struct Specialization<std::vector<UnityEngine::type>> { \
    using JSONType = std::vector<type>; \
};

SPECIALIZATION(Vector2)
SPECIALIZATION(Vector3)
SPECIALIZATION(Vector4)
SPECIALIZATION(Color)

#undef SPECIALIZATION

#pragma endregion

}

#endif