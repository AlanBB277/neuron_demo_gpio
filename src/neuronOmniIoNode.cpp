// Copyright 2017 ADLINK Technology, Inc.
// Developer: Alan Chen (alan.chen@adlinktech.com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include "neuronOmniIoNode.hpp"


/* * * * * * * * * * 
 * Private Methods *
 * * * * * * * * * */
void NeuronOmniIoNode::topic_callback(const std_msgs::msg::String::SharedPtr msg)
{
    // Print the received message
    printf("------------------------------------------------------------------\n");
    printf("=>>> receive from -- Topic <\"%s\">: \"%s\".\n", TOPIC_CMD, msg->data.c_str());
    printf("\n");

    // Check the availability of the SEMA library 
    if (NeuronOmniIo::IsAvailable() == false)
    {
        printf("[ERROR] NeuronOmniIoNode - SEMA Lib not found.\n");
        return;
    }

    // Toggle GPIO Level
    uint32_t level[4] = {0};
	uint32_t contact_sw_level, onoff_sw_level;
	uint32_t i;
	
	i = (i+1)%4;
	level[i] = EAPI_GPIO_HIGH;
	
    contact_sw_level = gpio_sw_contact_->ReadLevel(level);
	onoff_sw_level = gpio_sw_onoff_->ReadLevel(level);
	
    if (contact_sw_level == EAPI_GPIO_LOW)
	{
		for(j = 0;j<4;j++)
		{
			level[j] = EAPI_GPIO_HIGH;
		}
		
	}
	
	if (onoff_sw_level == EAPI_GPIO_HIGH)
	{
		// do something
	}
  
	set_led(level);

    /* publish data
	// Send it out
    std::string stmp;
    stmp += "The GPIO Pin";
    stmp += std::to_string(GPIO_TOGGLE_PIN);
    stmp += " is set to ";
    stmp += level? "HIGH" : "LOW";

    msg->data = stmp;
    printf("<<<= send to ------- Topic <\"%s\">: \"%s\".\n", TOPIC_DATA, msg->data.c_str());*/
    publisher_->publish(msg);    
    return;
}

void NeuronOmniIoNode::set_led(const uint32_t (&state)[4])
{
	gpio_led_r_->SetLevel(state[0]);
	gpio_led_o_->SetLevel(state[1]);
	gpio_led_y_->SetLevel(state[2]);
	gpio_led_g_->SetLevel(state[3]);
	return;
}


/* * * * * * * * * * 
 * Public Methods  *
 * * * * * * * * * */
NeuronOmniIoNode::NeuronOmniIoNode() : Node("neuron_gpio")
{
    publisher_ = this->create_publisher<std_msgs::msg::String>(
            TOPIC_DATA, rmw_qos_profile_sensor_data);

    subscription_ = this->create_subscription<std_msgs::msg::String>(
            TOPIC_CMD, std::bind(&NeuronOmniIoNode::topic_callback, this, _1),
            rmw_qos_profile_sensor_data);
        
    NeuronOmniIo::InitLib();
    
	if(NeuronOmniIo::IsAvailable())
	{
		//gpio_ = std::make_shared<NeuronOmniIo>(GPIO_TOGGLE_PIN);
		gpio_led_r_ = std::make_shared<NeuronOmniIo>(GPIO_LED_R_PIN);
		gpio_led_o_ = std::make_shared<NeuronOmniIo>(GPIO_LED_O_PIN);
		gpio_led_y_ = std::make_shared<NeuronOmniIo>(GPIO_LED_Y_PIN);
		gpio_led_g_ = std::make_shared<NeuronOmniIo>(GPIO_LED_G_PIN);
		gpio_sw_contact_ = std::make_shared<NeuronOmniIo>(GPIO_SW_CONTACT_PIN);
		gpio_sw_onoff_ = std::make_shared<NeuronOmniIo>(GPIO_SW_ONOFF_PIN);
		
		gpio_led_r_->SetDir(EAPI_GPIO_OUTPUT);
		gpio_led_o_->SetDir(EAPI_GPIO_OUTPUT);
		gpio_led_y_->SetDir(EAPI_GPIO_OUTPUT);
		gpio_led_g_->SetDir(EAPI_GPIO_OUTPUT);
		gpio_sw_contact_->SetDir(EAPI_GPIO_INPUT);
		gpio_sw_onoff_->SetDir(EAPI_GPIO_INPUT);
		
	}
}

NeuronOmniIoNode::~NeuronOmniIoNode()
{
    NeuronOmniIo::UnInitLib();
}