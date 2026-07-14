import numpy as np

if __name__ == "__main__":
    m = 0.346  # [kg]
    rpm = 200
    dmp = 0.68
    l = 3.5e-2
    h = 11.5e-2
    j = (1 / 12) * m * (h**2)
    r = 3.25e-2
    max_velocity = (rpm / 60) * 360
    k = (max_velocity * 0.1) / 255
    g = 9.81
    g_n = m * l * k
    g_d1 = -j + m * (l**2)
    g_d2 = m * g * l
    w_n = np.sqrt(m * l * g / g_d1)
    jt = g_d1 / g_n
    d = 2 * dmp * w_n * g_d1
    print(f"Massa: {m:.2f}")
    print(f"Distância até o centro de gravidade: {l:.3f}")
    print(f"Altura do robô: {h:.3f}")
    print(f"Momento de inércia [J]: {j:.3f}")
    print(f"K: {k:.3f}")
    print(f"w_n: {w_n:.3f}")

    print(
        f"G(s) = {g_n/g_d1:.2f} / {g_d1/g_d1:.2f} s² + {d/g_d1:.2f} s - {(g_d2)/g_d1:.2f}"
    )

    Kcr = (g_d2 / g_d1) / (g_n / g_d1)
    Tcr = 2 * np.pi / np.sqrt(g_d2 / g_d1)

    Kp = 0.6 * Kcr
    Ki = Kcr / (0.5 * Tcr)
    Kd = 0.125 * Kcr * Tcr

    print("---------------------")
    print("PID:")
    print(f"Kcr = {Kcr:.2f}")
    print(f"Tcr = {Tcr:.2f}")
    print(f"Kp = {Kp:.2f}")
    print(f"Ki = {Ki:.2f}")
    print(f"Kd = {Kd:.2f}")
