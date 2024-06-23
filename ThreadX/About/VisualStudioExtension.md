





```C#
    private async void PingDeviceCommandHandler(object sender, EventArgs arguments)
    {
        // yield to give the UI thread a chance to respond to user input
        await Task.Yield();

        await Task.Run(async delegate
        {
            var descriptionBackup = ViewModelLocator.DeviceExplorer.SelectedDevice.Description;

            MessageCentre.StartProgressMessage($"Pinging {descriptionBackup}...");
            try
            {
                // disable buttons
                await UpdateDeviceDependentToolbarButtonsAsync(false);

                // make sure this device is showing as selected in Device Explorer tree view
                ViewModelLocator.DeviceExplorer.ForceNanoDeviceSelection();

                // check if debugger engine exists
                if (NanoDeviceCommService.Device.DebugEngine == null)
                {
                    NanoDeviceCommService.Device.CreateDebugEngine();
                }

                // connect to the device
                if (!NanoDeviceCommService.Device.DebugEngine.Connect())
                {
                    MessageCentre.OutputMessage($"{descriptionBackup} is not responding, please reboot the device.");

                    return;
                }

                // ping device
                var reply = NanoDeviceCommService.Device.Ping();

                if (reply == ConnectionSource.nanoBooter)
                {
                    MessageCentre.OutputMessage($"{descriptionBackup} is active running nanoBooter.");
                }
                else if (reply == ConnectionSource.nanoCLR)
                {
                    MessageCentre.OutputMessage($"{descriptionBackup} is active running nanoCLR.");
                }
                else
                {
                    MessageCentre.OutputMessage($"No reply from {descriptionBackup}");
                }
            }
            catch
            {
                // OK to catch all
            }
            finally
            {
                // disconnect device
                NanoDeviceCommService.Device?.DebugEngine?.Stop(true);

                // enable buttons
                await UpdateDeviceDependentToolbarButtonsAsync(true);

                MessageCentre.StopProgressMessage();
            }
        });
```