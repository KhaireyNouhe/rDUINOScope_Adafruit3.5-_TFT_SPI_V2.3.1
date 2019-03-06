////////////////////////// Touch events:
// Page change event:
void BootPushCallback(void *ptr)  // If page 0 is loaded on the display, the following is going to execute:
{
  CurrentPage = 0;  // Set variable as 0 so from now on arduino knows page 0 is loaded on the display
}  // End of press event

void GPSPushCallback(void *ptr)  // If page 0 is loaded on the display, the following is going to execute:
{
  CurrentPage = 1;  // Set variable as 0 so from now on arduino knows page 0 is loaded on the display
}
