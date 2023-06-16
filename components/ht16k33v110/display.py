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
CONF_INTENSITY_VALUES = "intensity_values"
CONF_SENSOR_VALUES = "sensor_values"

ht16k33v110_ns = cg.esphome_ns.namespace("ht16k33v110")
HT16K33V110Display = ht16k33v110_ns.class_("HT16K33V110Display", cg.PollingComponent, i2c.I2CDevice)
HT16K33V110DisplayRef = HT16K33V110Display.operator("ref")

ht16k33v110_intensity_sensor_values = []
ht16k33v110_intensity_values = []


def validate_intensity(config):
    if (CONF_INTENSITY_MAP in config and CONF_INTENSITY in config):
        raise cv.Invalid(
          f"Do not specify {CONF_INTENSITY} when using {CONF_INTENSITY_MAP}"
        )
        
def validate_intensity_map(config):
    if isinstance(value, list):
        value = cv.string(value)
        parts = value.split("->")
        if len(parts) != 2:
            raise cv.Invalid(" Intensity map parameter must be of form 3000 -> 2")
        sensor_value = cv.float(parts[0].strip())
        intensity_value = cv.float(parts[0].strip())
        ht16k33v110_intensity_sensor_values.append(sensor_value)
        ht16k33v110_intensity_values.append(intensity_value)
        value = { CONF_SENSOR_VALUES: ht16k33v110_intensity_sensor_values,
                   CONF_INTENSITY_VALUES: ht16k33v110_intensity_values}
    else:
       raise cv.Invalid(" Intensity map parameter must be of form 3000 -> 2")
    return (cv.Schema(
        {
            cv.Required(CONF_SENSOR_VALUES): cv.ensure_list,
            cv.Required(CONF_INTENSITY_VALUES): cv.ensure_list,
        }(value))
    )
        
INTENSITY_MAP_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_SOURCE_ID): cv.use_id(sensor.Sensor),
        cv.Required(CONF_MAP): validate_intensity_map,
    }
)
        
CONFIG_SCHEMA = (
    validate_intensity,
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(HT16K33V110Display),
            cv.Optional(CONF_INTENSITY, default=7): cv.All(
                cv.uint8_t, cv.Range(min=1, max=16)
            ),
            cv.Optional(CONF_INVERTED, default=False): cv.boolean,
            cv.Optional(CONF_INTENSITY_MAP): cv.maybe_simple_value(INTENSITY_MAP_SCHEMA),
        }
    ).extend(cv.polling_component_schema("1s"))
    .extend(i2c.i2c_device_schema(0x70))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_intensity(config[CONF_INTENSITY]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(HT16K33V110DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
