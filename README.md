# Falcon: Honest-Majority Maliciously Secure Framework for Private Deep Learning and Fixed Key Random Oracle

This project is a fork of [Falcon framework](https://github.com/snwagh/falcon-public.git) implementation, integrating the setting of [the random oracle](https://github.com/anbaccar/RSS_ring_ppml/blob/master/include/randBit.h) as demonstrated in [RingPPML](https://github.com/anbaccar/RSS_ring_ppml.git).


### Table of Contents

- [Warning](#warning)
- [Requirements](#requirements)
- [Source Code](#source-code)
    - [Repository Structure](#repository-structure)
    - [Building the code](#building-the-code)
    - [Running the code](#running-the-code)
- [Additional Resources](#additional-resources)
    - [Comparison with RingPPML](#comparison-with-ringppml)
    - [Citation](#citation)

<!-- ### Table of Contents

- [Warning](#warning)
- [Requirements](#requirements)
- [Source Code](#source-code)
    - [Repository Structure](#repository-structure)
    - [Building the code](#building)
    - [Running the code](#running)
- [Additional Resources](#additional-resources)
    - [Run combinations](#run-combinations)
    - [Comparison with SecureNN](#comparison-with-securenn)
    - [Errors and Issues](#errors-and-issues)
    - [Todos](#todos)
    - [Citation](#citation) -->


### Warning
---

With a base warning displayed in the original repository of the [Falcon framework](https://github.com/snwagh/falcon-public.git), we note that this is the final project intended to support studying.

### Requirements
---
* The code should work on most Linux distributions (It has been developed and tested with [Ubuntu](http://www.ubuntu.com/) 16.04 and 18.04).

* **Required packages for Falcon:**
  * [`g++`](https://packages.debian.org/testing/g++)
  * [`make`](https://packages.debian.org/testing/make)
  * [`libssl-dev`](https://packages.debian.org/testing/libssl-dev)

  Install these packages with your favorite package manager, e.g, `sudo apt-get install <package-name>`.

* For more details and up-to-date information, please refer to the original [falcon-framework](https://github.com/snwagh/falcon-public.git)
### Docker 
---

To install and run Falcon using docker by downloading the [Dockerfile](https://github.com/trhieung/falcon-public/blob/master/Dockerfile) in this project, 
- first build the container:
` docker build -t falcon .`
- Then run the container
`docker run -it falcon '/bin/bash'`
- Use tmux and the following command combination `Ctrl+b %` to create 3 windows for demonstrating with 3 parties.
- Each window represents the ID of the respective parties. Run the following commands in each window to get the results

#### Party 0
```
./Falcon.out 0 files/IP_localhost files/keyA files/keyAB files/keyAC LeNet MNIST Semi-honest
```
#### Party 1
```
./Falcon.out 1 files/IP_localhost files/keyB files/keyBC files/keyAB LeNet MNIST Semi-honest
```
#### Party 2
```
./Falcon.out 2 files/IP_localhost files/keyC files/keyAC files/keyBC LeNet MNIST Semi-honest
```

### Source Code
---

#### Repository Structure

* `src/Ranbit.h` and `src/Ranbit.cpp` - The additions we contributed

#### Building the code

To build Falcon, run the following commands:

```
git clone https://github.com/trhieung/falcon-public Falcon
cd Falcon
make all -j$(nproc)
```

#### Running the code

To run the code, please refer to the original [falcon-framework](https://github.com/snwagh/falcon-public.git) or follow the steps with [docker](#docker) for testing purposes

### Additional Resources
---

#### Comparison with [RingPPML](https://github.com/anbaccar/RSS_ring_ppml.git)
| Element | Falcon    | RingPPML    |
| :---:   | :---: | :---: |
| Data Transfer | Not supported: encrypt before sending or decrypt when receiving data <br> (as this is just for benchmarking) | Supported: AES encryption and decryption with block size, [implementation](https://github.com/anbaccar/RSS_ring_ppml/blob/7b9483e203b8f9547532cef6cd446d40e9947a7b/src/connection/NodeNetwork.cpp#L399)   |
| Computation Base | some protocol base on field modulo, [implentation](https://github.com/snwagh/falcon-public/blob/master/src/tools.h#L193) | All computation on Ring $Z_{2^k}$, speeding up the computation cost of modulo   |

#### Citation
You can cite the paper using the following bibtex entry (the paper links to this origin repo):
```
@inproceedings{wagh2021falcon,
  title={{FALCON: Honest-Majority Maliciously Secure Framework for Private Deep Learning}},
  author={Wagh, Sameer and Tople, Shruti and Benhamouda, Fabrice and Kushilevitz, Eyal and Mittal, Prateek and Rabin, Tal},
  journal={Proceedings on Privacy Enhancing Technologies},
  year={2021}
}
```
or cite RingPPML paper with by 
```
@article{baccarini2023rss,
  title={Multi-Party Replicated Secret Sharing over a Ring with Applications to Privacy-Preserving Machine Learning},
  author={Baccarini, Alessandro and Blanton, Marina and Yuan, Chen},
  journal={Proceedings on Privacy Enhancing Technologies (PoPETs)},
  volume = 2023,
  number = 1,
  pages={608-626}, 
  year={2023}
}
```
