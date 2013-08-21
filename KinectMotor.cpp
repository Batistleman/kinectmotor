  #include "KinectMotor.h"
  #include <libusb-1.0/libusb.h>

  using namespace std;

  namespace PJLib
  {
    KinectMotor::KinectMotor()
    {
          /* Vendor ID and Product ID define the usb device */
      short VENDOR_ID = (short)0x045E;
          //short PRODUCT_ID = (short)0x02B0;
      short PRODUCT_ID = (short)0x02c2;

          /* Initialize the usb communication */
          libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
          libusb_context *ctx = NULL; //a libusb session

          ssize_t cnt;
          int r = libusb_init(&ctx);
          if(r < 0) {
            cout << "Init Error" << r << endl;
            return;
        }

          libusb_set_debug(ctx, 3); //set verbosity level to 3 (as suggested in the documentation)
          cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
          if(cnt < 0) {
              cout<<"Get Device Error"<<endl; //there was an error
          }

          for(int i = 0; i < cnt; i++) {

              libusb_device *dev = devs[i];

              libusb_device_descriptor desc;
              int r = libusb_get_device_descriptor(dev, &desc);
              if (r < 0) {
                cout<<"failed to get device descriptor"<<endl;
                return;
            }
            cout<<"Number of possible configurations: "<<(int)desc.bNumConfigurations<<"  ";
            cout<<"Device Class: "<<(int)desc.bDeviceClass<<"  ";
            cout<<"VendorID: "<<desc.idVendor<<"  ";
            cout<<"ProductID: "<<desc.idProduct<<endl;
            libusb_config_descriptor *config;
            libusb_get_config_descriptor(dev, 0, &config);
            cout<<"Interfaces: "<<(int)config->bNumInterfaces<<" ||| ";
            const libusb_interface *inter;
            const libusb_interface_descriptor *interdesc;
            const libusb_endpoint_descriptor *epdesc;
            for(int i=0; i<(int)config->bNumInterfaces; i++) {
                inter = &config->interface[i];
                cout<<"Number of alternate settings: "<<inter->num_altsetting<<" | ";
                for(int j=0; j<inter->num_altsetting; j++) {
                  interdesc = &inter->altsetting[j];
                  cout<<"Interface Number: "<<(int)interdesc->bInterfaceNumber<<" | ";
                  cout<<"Number of endpoints: "<<(int)interdesc->bNumEndpoints<<" | ";
                  for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
                    epdesc = &interdesc->endpoint[k];
                    cout<<"Descriptor Type: "<<(int)epdesc->bDescriptorType<<" | ";
                    cout<<"EP Address: "<<(int)epdesc->bEndpointAddress<<" | ";
                }
            }
        }
        cout<<endl<<endl<<endl;
        libusb_free_config_descriptor(config);

    }

          libusb_free_device_list(devs, 1); //free the list, unref the devices in it
          libusb_exit(ctx); //close the session


         /* libusb_find_busses();
          libusb_find_devices();
          busses = libusb_get_busses();

          // loop through the devices until the kinect motor was found
          for (bus = busses; bus; bus = bus->next)
          {
              for (dev = bus->devices; dev; dev = dev->next)
              {
                  if ((dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == PRODUCT_ID))
                  {
                      break;
                  }
              }
          }

          if(dev == NULL)
          {
              throw exception("Kinect motor could not be found!");
          }

          // Open device
          try
          {
              usbHandle = usb_open(dev);
          }
          catch (...)
          {
              throw exception("Device could not be opened!");
          }*/

          }

          int KinectMotor::sendMessage(int requestType, int request, int value, char *data, int size)
          {
              int rval = -1;
              /*try
              {
                  rval = libusb_control_msg(usbHandle, requestType, request, value, 0, data, size, 2000);

                  if(rval < 0)
                  {
                      throw exception("Error when sending message to kinect motor! (rval < 0)");
                  }
              }
              catch (...)
              {
                  throw exception("Error when sending message to kinect motor!");
              }*/

                  return rval;
              }

              bool KinectMotor::isReady()
              {
              char *buf = new char[1];                        // One-element empty array
              int rval = sendMessage(0xC0, 0x10, 0, buf, 1);
              if(rval == -1)
                return false;
          return (buf[0] == 0x22);                        // 0x22 means the motor is ready
      }

      void KinectMotor::Close()
      {
          if(usbHandle != NULL)
            libusb_close(usbHandle);
    }

    char* KinectMotor::GetMotorInfo()
    {
      char* buf = new char[10];
      this->sendMessage(0xC0, 0x32, 0, buf, 10);
      return buf;
  }

  int* KinectMotor::GetAccelerometerData()
  {
      char* buf = this->GetMotorInfo();
      int* accel = new int[3];

          accel[0] = (int) (((short)buf[2] << 8) | buf[3]);   // X
          accel[1] = (int) (((short)buf[4] << 8) | buf[5]);   // Y
          accel[2] = (int) (((short)buf[6] << 8) | buf[7]);   // Z

          return accel;
      }

      double* KinectMotor::GetAccelerometerGForce()
      {
          double ACCEL_COUNT = 819.0;

          int* accel = this->GetAccelerometerData();
          double* accelG = new double[3];

          accelG[0] = accel[0]/ACCEL_COUNT;   // X
          accelG[1] = accel[1]/ACCEL_COUNT;   // Y
          accelG[2] = accel[2]/ACCEL_COUNT;   // Z

          return accelG;
      }
  }