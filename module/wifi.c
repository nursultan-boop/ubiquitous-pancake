#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nursultan");
MODULE_DESCRIPTION("WiFi scanner kernel module");

static int __init wifi_scanner_init(void)
{
    struct net_device *dev;
    struct iwreq wrq;
    int sockfd, num_scan;
    char buffer[IW_SCAN_MAX_DATA];

    printk(KERN_INFO "WiFi scanner module initialized\n");

    // Get the wireless device
    dev = first_net_device(&init_net);
    while (dev) {
        if (dev->ieee80211_ptr) {
            break;
        }
        dev = next_net_device(dev);
    }

    if (!dev) {
        printk(KERN_ERR "No wireless device found\n");
        return -ENODEV;
    }

    // Open a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printk(KERN_ERR "Error opening socket\n");
        return sockfd;
    }

    // Set up the scan request
    memset(&wrq, 0, sizeof(wrq));
    strncpy(wrq.ifr_name, dev->name, IFNAMSIZ);
    wrq.u.data.pointer = buffer;
    wrq.u.data.length = IW_SCAN_MAX_DATA;
    wrq.u.data.flags = 0;

    // Perform the scan
    if (ioctl(sockfd, SIOCSIWSCAN, &wrq) < 0) {
        printk(KERN_ERR "Error performing scan\n");
        close(sockfd);
        return -EFAULT;
    }

    // Get the scan results
    memset(buffer, 0, sizeof(buffer));
    wrq.u.data.pointer = buffer;
    if (ioctl(sockfd, SIOCGIWSCAN, &wrq) < 0) {
        printk(KERN_ERR "Error getting scan results\n");
        close(sockfd);
        return -EFAULT;
    }

    // Parse the scan results
    num_scan = (wrq.u.data.length - sizeof(struct iw_event)) / sizeof(struct iw_scan_info);
    if (num_scan > 0) {
        int i;
        struct iw_scan_info *info = (struct iw_scan_info *)(buffer + sizeof(struct iw_event));
        printk(KERN_INFO "Found %d WiFi networks:\n", num_scan);
        for (i = 0; i < num_scan; i++) {
            printk(KERN_INFO "  %s\n", info[i].essid);
        }
    } else {
        printk(KERN_INFO "No WiFi networks found\n");
    }

    // Clean up
    close(sockfd);

    return 0;
}

static void __exit wifi_scanner_exit(void)
{
    printk(KERN_INFO "WiFi scanner module exited\n");
}

module_init(wifi_scanner_init);
module_exit(wifi_scanner_exit);
