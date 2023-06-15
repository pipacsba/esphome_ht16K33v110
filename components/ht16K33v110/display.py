import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, i2c
from esphome.const import (
    CONF_LAMBDA,
    CONF_INTENSITY,
    CONF_INVERTED,
    CONF_LENGTH,
)

ht16k33v110_ns = cg.esphome_ns.namespace("ht16k33v110")
HT16K33V110Display = ht16k33v110_ns.class_("ht16k33v110Display", cg.PollingComponent)
HT16K33V110DisplayRef = HT16K33V110Display.operator("ref")

CONFIG_SCHEMA = (
    display.BASIC_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(HT16K33V110Display),
            cv.Optional(CONF_INTENSITY, default=7): cv.All(
                cv.uint8_t, cv.Range(min=1, max=16)
            ),
            cv.Optional(CONF_INVERTED, default=False): cv.boolean, # future improvement
        }
    ).extend(cv.polling_component_schema("1s"))
    .extend(i2c.i2c_device_schema(0x70))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)

    cg.add(var.set_intensity(config[CONF_INTENSITY]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(TM1637DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
