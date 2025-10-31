import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, climate, select, switch as switch_
from esphome.const import (
    CONF_ID,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["sensor", "text_sensor", "binary_sensor"]

CONF_ATREA_ID = "atrea_id"
CONF_THERMOSTAT_ID = "thermostat_id"
CONF_ESP_INTENSITY_ID = "esp_intensity_id"
CONF_ESP_MODE_ID = "esp_mode_id"
CONF_ESP_HEATING_ID = "esp_heating_id"
CONF_ESP_COOLING_ID = "esp_cooling_id"

atrea_ns = cg.esphome_ns.namespace("atrea")
AtreaComponent = atrea_ns.class_("AtreaComponent", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(AtreaComponent),
            cv.Required(CONF_THERMOSTAT_ID): cv.use_id(climate.Climate),
            cv.Required(CONF_ESP_INTENSITY_ID): cv.use_id(select.Select),
            cv.Required(CONF_ESP_MODE_ID): cv.use_id(select.Select),
            cv.Required(CONF_ESP_HEATING_ID): cv.use_id(switch_.Switch),
            cv.Required(CONF_ESP_COOLING_ID): cv.use_id(switch_.Switch),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    thermostat = await cg.get_variable(config[CONF_THERMOSTAT_ID])
    cg.add(var.set_thermostat(thermostat))

    intensity = await cg.get_variable(config[CONF_ESP_INTENSITY_ID])
    cg.add(var.set_esp_intensity(intensity))

    mode = await cg.get_variable(config[CONF_ESP_MODE_ID])
    cg.add(var.set_esp_mode(mode))

    heating = await cg.get_variable(config[CONF_ESP_HEATING_ID])
    cg.add(var.set_esp_heating(heating))

    cooling = await cg.get_variable(config[CONF_ESP_COOLING_ID])
    cg.add(var.set_esp_cooling(cooling))
