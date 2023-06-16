import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, i2c, sensor
from esphome.const import (
    CONF_LAMBDA,
    CONF_INTENSITY,
    CONF_INVERTED,
    CONF_LENGTH,
    CONF_ID,
)

CONF_INTENSITY_MAP = "intensity_map"
CONF_SOURCE_ID = "source_id"
CONF_MAP = "map"
CONF_INTENSITY_VALUE = "intensity_values"
CONF_SENSOR_VALUE = "sensor_values"

ht16k33v110_ns = cg.esphome_ns.namespace("ht16k33v110")
HT16K33V110Display = ht16k33v110_ns.class_("HT16K33V110Display", cg.PollingComponent, i2c.I2CDevice)
HT16K33V110DisplayRef = HT16K33V110Display.operator("ref")

def validate_intensity(config):
    if (CONF_INTENSITY_MAP in config and CONF_INTENSITY in config):
        raise cv.Invalid(
          f"Do not specify {CONF_INTENSITY} when using {CONF_INTENSITY_MAP}"
        )
    return config

def validate_map(value):
    if isinstance(value, dict):
        return cv.Schema(
            {
                cv.Required(CONF_INTENSITY_VALUE): cv.float_,
                cv.Required(CONF_SENSOR_VALUE): cv.float_,
            }
        )(value)

    value = cv.string(value)
    parts = value.split("->")
    if len(parts) != 2:
        raise cv.Invalid("Calibration parameter must be of form 3000 -> 23°C")
    sensor_value = float(parts[0].strip())
    intensity = float(parts[1].strip())
    return validate_map(
        {
            CONF_INTENSITY_VALUE: intensity,
            CONF_SENSOR_VALUE: sensor_value,
        }
    )

KT16K33V110_SCHEMA = cv.Schema(
    display.BASIC_DISPLAY_SCHEMA
    .extend(
        {
            cv.GenerateID(): cv.declare_id(HT16K33V110Display),
            cv.Optional(CONF_INTENSITY, default=7): cv.All(
                cv.uint8_t, cv.Range(min=1, max=16)
            ),
            cv.Optional(CONF_INVERTED, default=False): cv.boolean,
            cv.Optional(CONF_INTENSITY_MAP): cv.Schema(
                {
                    cv.Required(CONF_SOURCE_ID): cv.use_id(sensor.Sensor),
                    cv.Required(CONF_MAP): cv.ensure_list(validate_map),
                }
            ),
        }
    )
    .extend(i2c.i2c_device_schema(0x70))
    .extend(cv.polling_component_schema("1s"))
)

CONFIG_SCHEMA = cv.All(validate_intensity, KT16K33V110_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_intensity(config[CONF_INTENSITY]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    if CONF_INTENSITY_MAP in config:
        #cg.add(var.set_intensity_map(config[CONF_INTENSITY_MAP][CONF_MAP]))
        intensity_source_values = []
        intensity_values = []
        for a_value in config[CONF_INTENSITY_MAP][CONF_MAP]:
            intensity_source_values.append(a_value[CONF_SENSOR_VALUE])
            intensity_values.append(a_value[CONF_SENSOR_VALUE])
        cg.add(set_intensity_source_values(intensity_source_values)
        cg.add(set_intensity_values(intensity_values)
        cg.add(var.set_intensity_auto())
        cg.add(var.set_intensity_sensor_id(config[CONF_INTENSITY_MAP][CONF_SOURCE_ID]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(HT16K33V110DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
