import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
)
from . import atrea_ns, AtreaComponent, CONF_ATREA_ID

CONF_OUTDOOR_TEMPERATURE = "outdoor_temperature"
CONF_EXCHANGER_TEMPERATURE = "exchanger_temperature"
CONF_RECUPERATOR_TEMPERATURE = "recuperator_temperature"
CONF_CIRCULATION = "circulation"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ATREA_ID): cv.use_id(AtreaComponent),
        cv.Optional(CONF_OUTDOOR_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_EXCHANGER_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_RECUPERATOR_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CIRCULATION): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ATREA_ID])

    if outdoor_temp_config := config.get(CONF_OUTDOOR_TEMPERATURE):
        sens = await sensor.new_sensor(outdoor_temp_config)
        cg.add(parent.set_outdoor_temperature(sens))

    if exchanger_temp_config := config.get(CONF_EXCHANGER_TEMPERATURE):
        sens = await sensor.new_sensor(exchanger_temp_config)
        cg.add(parent.set_exchanger_temperature(sens))

    if recuperator_temp_config := config.get(CONF_RECUPERATOR_TEMPERATURE):
        sens = await sensor.new_sensor(recuperator_temp_config)
        cg.add(parent.set_recuperator_temperature(sens))

    if circulation_config := config.get(CONF_CIRCULATION):
        sens = await sensor.new_sensor(circulation_config)
        cg.add(parent.set_circulation(sens))