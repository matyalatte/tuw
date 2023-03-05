# How to Build With Github Actions

[Github Actions](https://docs.github.com/en/actions) is an automation tool for development workflows.  
It can build and upload the executable from any branches and tags.  
But please note that the available time for Github Actions is 2000 minutes per month for free account.  

## 1. Fork the repository

<img src=https://user-images.githubusercontent.com/69258547/154954141-659470f1-695c-47ea-9923-c09ae9cd1931.png width=500>

## 2. Select your account

<img src=https://user-images.githubusercontent.com/69258547/154954597-0e3dd351-7c37-433d-99e8-90fe7ddbb59f.png width=500>

## 3. Go to actions tab

<img src=https://user-images.githubusercontent.com/69258547/154954603-e70039cb-cf34-484b-816f-2fee487e53ac.png width=500>

## 4. Enable actions

<img src=https://user-images.githubusercontent.com/69258547/154954954-2816f461-eb5d-4813-bdc3-f37ce2feeee3.png width=500>

## 5. Select `build_*` from workflows

-   [`build_windows`](../.github/workflows/build_windows.yml) is for Windows.
-   [`build_mac`](../.github/workflows/build_mac.yml) is for MacOS.
-   [`build_ubuntu`](../.github/workflows/build_ubuntu.yml) is for Ubuntu (20.04 or later).

<img src=https://user-images.githubusercontent.com/69258547/154954961-1bb8b185-c463-4241-b5d2-d2988cbb08be.png width=500>

## 6. Select a branch or a tag

Select a version you want to use.  
![select](https://user-images.githubusercontent.com/69258547/177385263-7c7a6caa-7a2e-4626-9c9b-406672f3c964.png)

## 7. Run workflow

<img src=https://user-images.githubusercontent.com/69258547/154954963-de84f8c1-2519-463f-8393-84d161f3c828.png width=500>

## 8. Wait until it finishes the workflow

It'll take about 10 minutes.  
<img src=https://user-images.githubusercontent.com/69258547/154955296-cf3db44b-5fec-4674-91f3-4ddc3c1a4042.png width=500>

## 9. Go to release page

Github will upload a zip file on the release page.  
<img src=https://user-images.githubusercontent.com/69258547/154955299-11814f01-7ef6-4466-885b-de55315098bf.png width=500>

## 10. Download zip

Download `SimpleCommandRunner*.zip` or `*.tar.bz2`.  

<img src=https://user-images.githubusercontent.com/69258547/154955303-45705ac4-cf93-4b93-bf7f-cf82537d6c71.png width=500>