void sendToDisplay1(String command)   //send Data to Display 1
{
  Serial1.print(command);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
} 

void sendToDisplay2(String command)   //send Data to Display 2
{
  Serial2.print(command);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
} 
