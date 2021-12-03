#include "esp_err.h"
#include "nvs_flash.h"

#include "Wifi.h"
#include "SensorPose.h"
#include "Marvelmind.h"
#include "data_types.h"
#include "NodeHandle.h"
#include "Publisher.h"
#include "ros_msgs.h"

using namespace MARVELMIND;
using namespace WIFI;


extern "C" void app_main(void)
{   
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    Wifi wifi{};

    ESP_ERROR_CHECK(wifi.init());
    ESP_ERROR_CHECK(wifi.begin());

    ros::NodeHandle nh("robot_1");

    ros::Publisher pub = nh.advertise<ros_msgs::Pose2D>("pose2D");

    SensorPose *pose_sensor = new Marvelmind();
    ESP_ERROR_CHECK(pose_sensor->init());

    while(1) 
    { 
        data_types::Pose2D x = pose_sensor->get_Pose();
        printf("X: %f, Y: %f, Theta: %f\n", x.x, x.y,  x.theta);

        char measurement[128];
        sprintf(measurement, "X: %f, Y: %f, Theta: %f", x.x, x.y,  x.theta);

        ros_msgs::Pose2D pose(x.x, x.y, x.theta);

        pub.publish(pose);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
