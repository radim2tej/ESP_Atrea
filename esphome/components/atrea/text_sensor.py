import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID
from . import atrea_ns, AtreaComponent, CONF_ATREA_ID

CONF_CP07_MODE = "cp07_mode"
CONF_CP07_INTENSITY = "cp07_intensity"
CONF_CP07_TEMPERATURE_CONTROL = "cp07_temperature_control"
CONF_CP07_BYPASS = "cp07_bypass"
CONF_ATREA_MODE = "atrea_mode"
CONF_ATREA_INTENSITY = "atrea_intensity"
CONF_ATREA_ERRORS = "atrea_errors"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ATREA_ID): cv.use_id(AtreaComponent),
        cv.Optional(CONF_CP07_MODE): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_CP07_INTENSITY): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_CP07_TEMPERATURE_CONTROL): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_CP07_BYPASS): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_ATREA_MODE): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_ATREA_INTENSITY): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_ATREA_ERRORS): text_sensor.text_sensor_schema(),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ATREA_ID])

    if cp07_mode_config := config.get(CONF_CP07_MODE):
        sens = await text_sensor.new_text_sensor(cp07_mode_config)
        cg.add(parent.set_cp07_mode(sens))

    if cp07_intensity_config := config.get(CONF_CP07_INTENSITY):
        sens = await text_sensor.new_text_sensor(cp07_intensity_config)
        cg.add(parent.set_cp07_intensity(sens))

    if cp07_temperature_control_config := config.get(CONF_CP07_TEMPERATURE_CONTROL):
        sens = await text_sensor.new_text_sensor(cp07_temperature_control_config)
        cg.add(parent.set_cp07_temperature_control(sens))

    if cp07_bypass_config := config.get(CONF_CP07_BYPASS):
        sens = await text_sensor.new_text_sensor(cp07_bypass_config)
        cg.add(parent.set_cp07_bypass(sens))

    if atrea_mode_config := config.get(CONF_ATREA_MODE):
        sens = await text_sensor.new_text_sensor(atrea_mode_config)
        cg.add(parent.set_atrea_mode(sens))

    if atrea_intensity_config := config.get(CONF_ATREA_INTENSITY):
        sens = await text_sensor.new_text_sensor(atrea_intensity_config)
        cg.add(parent.set_atrea_intensity(sens))

    if atrea_errors_config := config.get(CONF_ATREA_ERRORS):
        sens = await text_sensor.new_text_sensor(atrea_errors_config)
        cg.add(parent.set_atrea_errors(sens))
