import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID
from . import atrea_ns, AtreaComponent, CONF_ATREA_ID

CONF_ATREA_HEATING = "atrea_heating"
CONF_ATREA_COOLING = "atrea_cooling"
CONF_ATREA_BOOST_VENTILATION = "atrea_boost_ventilation"
CONF_ATREA_D1 = "atrea_d1"
CONF_ATREA_D2 = "atrea_d2"
CONF_ATREA_D3 = "atrea_d3"
CONF_ATREA_D4 = "atrea_d4"
CONF_ATREA_DAMPER_SR = "atrea_damper_sr"
CONF_ATREA_BYPASS_SB = "atrea_bypass_sb"
CONF_ATREA_YV = "atrea_yv"
CONF_ATREA_K = "atrea_k"
CONF_ATREA_OC1 = "atrea_oc1"
CONF_ESP_ACTIVE_CONTROLLER = "esp_active_controller"
CONF_ESP_HEATING_SEASON = "esp_heating_season"
CONF_ESP_BYPASS = "esp_bypass"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ATREA_ID): cv.use_id(AtreaComponent),
        cv.Optional(CONF_ATREA_HEATING): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_COOLING): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_BOOST_VENTILATION): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_D1): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_D2): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_D3): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_D4): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_DAMPER_SR): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_BYPASS_SB): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_YV): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_K): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ATREA_OC1): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ESP_ACTIVE_CONTROLLER): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ESP_HEATING_SEASON): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_ESP_BYPASS): binary_sensor.binary_sensor_schema(),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ATREA_ID])

    if atrea_heating_config := config.get(CONF_ATREA_HEATING):
        sens = await binary_sensor.new_binary_sensor(atrea_heating_config)
        cg.add(parent.set_atrea_heating(sens))

    if atrea_cooling_config := config.get(CONF_ATREA_COOLING):
        sens = await binary_sensor.new_binary_sensor(atrea_cooling_config)
        cg.add(parent.set_atrea_cooling(sens))

    if atrea_boost_config := config.get(CONF_ATREA_BOOST_VENTILATION):
        sens = await binary_sensor.new_binary_sensor(atrea_boost_config)
        cg.add(parent.set_atrea_boost_ventilation(sens))

    if atrea_d1_config := config.get(CONF_ATREA_D1):
        sens = await binary_sensor.new_binary_sensor(atrea_d1_config)
        cg.add(parent.set_atrea_d1(sens))

    if atrea_d2_config := config.get(CONF_ATREA_D2):
        sens = await binary_sensor.new_binary_sensor(atrea_d2_config)
        cg.add(parent.set_atrea_d2(sens))

    if atrea_d3_config := config.get(CONF_ATREA_D3):
        sens = await binary_sensor.new_binary_sensor(atrea_d3_config)
        cg.add(parent.set_atrea_d3(sens))

    if atrea_d4_config := config.get(CONF_ATREA_D4):
        sens = await binary_sensor.new_binary_sensor(atrea_d4_config)
        cg.add(parent.set_atrea_d4(sens))

    if atrea_damper_config := config.get(CONF_ATREA_DAMPER_SR):
        sens = await binary_sensor.new_binary_sensor(atrea_damper_config)
        cg.add(parent.set_atrea_damper_sr(sens))

    if atrea_bypass_config := config.get(CONF_ATREA_BYPASS_SB):
        sens = await binary_sensor.new_binary_sensor(atrea_bypass_config)
        cg.add(parent.set_atrea_bypass_sb(sens))

    if atrea_yv_config := config.get(CONF_ATREA_YV):
        sens = await binary_sensor.new_binary_sensor(atrea_yv_config)
        cg.add(parent.set_atrea_yv(sens))

    if atrea_k_config := config.get(CONF_ATREA_K):
        sens = await binary_sensor.new_binary_sensor(atrea_k_config)
        cg.add(parent.set_atrea_k(sens))

    if atrea_oc1_config := config.get(CONF_ATREA_OC1):
        sens = await binary_sensor.new_binary_sensor(atrea_oc1_config)
        cg.add(parent.set_atrea_oc1(sens))

    if esp_active_controller_config := config.get(CONF_ESP_ACTIVE_CONTROLLER):
        sens = await binary_sensor.new_binary_sensor(esp_active_controller_config)
        cg.add(parent.set_esp_active_controller(sens))

    if esp_heating_season_config := config.get(CONF_ESP_HEATING_SEASON):
        sens = await binary_sensor.new_binary_sensor(esp_heating_season_config)
        cg.add(parent.set_esp_heating_season(sens))

    if esp_bypass_config := config.get(CONF_ESP_BYPASS):
        sens = await binary_sensor.new_binary_sensor(esp_bypass_config)
        cg.add(parent.set_esp_bypass(sens))
