# iosig
IOSIG: Declarative I/O-Stream Properties Using Pragmas

Non-functional properties of IO-streams are typically not specified and passed to the used middleware or operating system. Knowing properties such as the expected access pattern, reliability, and visibility for data would allow a better storage resource selection by the infrastructure and thus could improve overall performance. With pragma annotations, we let developers declare the intended data access for their file descriptors and automatically redirect the corresponding input/output to the most appropriate storage device of the user’s system. Our automatic resource selection lets applications leverage system storage capabilities better, often delivers improved performance (in some cases by orders of magnitude), and relieves uninformed users from having to make the best choice manually.

This work is published as follows:
Gholami, M., Schintke, F. IOSIG: Declarative I/O-Stream Properties Using Pragmas. Datenbank Spektrum 22, 109–119 (2022). https://doi.org/10.1007/s13222-022-00419-w
