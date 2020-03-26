#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define N 10000
#define PI 3.14159265

const char *me = "bin/gen";

int
main()
{
    FILE *fp;
    int i, j;
    int aleatorio = 1;
    int carbono = 1;
    int red = 1;
    int esfera = 1;
    int onda = 1;
    int unaD = 1;
    int lisa = 1;
    int fuente = 1;
    int fuente2 = 0;
    int colors = 1;
    int infinito = 1;
    int infinito2 = 1;
    int campo2D = 1;
    int campo3D = 1;
    int stars = 1;
    int all = 1;
    char DATADIR[8] = "dat/";
    char name[20];

    if (aleatorio) {
        strcpy(name, DATADIR);
        strcat(name, "aleatori03d.dat");
        fp = fopen(name, "wt");
        if (fp == NULL) {
            fprintf(stderr, "%s: fail to open '%s'\n", me, name);
            exit(2);
        }

        for (i = 0; i < 800; i++) {
            fprintf(fp, "%f %f %f %f %d\n",
                    (float) (rand() % 1000) / 100,
                    (float) (rand() % 1000) / 100,
                    (float) (rand() % 1000) / 100,
                    (float) (rand() % 1000) / 1000, rand() % 1024);
        }
        fprintf(fp, "\n");
        fclose(fp);
    }
    if (carbono) {

        strcpy(name, DATADIR);
        strcat(name, "carbono.dat");
        fp = fopen(name, "wt");

        fprintf(fp, "%f %f %f %d %d\n", 0., 0., 0., 1, 2);
        fprintf(fp, "%f %f %f %d %d\n", 1., 0., 0., 1, 2);
        fprintf(fp, "%f %f %f %d %d\n", .5, .866, 0., 1, 2);
        fprintf(fp, "%f %f %f %d %d\n", .5, .433, .866, 1, 2);
        fprintf(fp, "\n");
        fclose(fp);
    }
    if (red) {
        float x, y, z, x0;
        float inc = .1;
        int cont = 0;

        strcpy(name, DATADIR);
        strcat(name, "red3d.dat");
        fp = fopen(name, "wt");

        x = y = z = 0;
        for (z = 0; z < .5; z += inc) {
            cont++;
            if (cont % 2)
                x0 = inc / 2;
            else
                x0 = 0;
            for (y = x0; y < 1; y += inc) {
                for (x = x0; x < 1.5; x += inc) {
                    fprintf(fp, "%f %f %f %f\n", x, y, z, inc / 2.);
                }
            }
        }
        fprintf(fp, "\n");
        fclose(fp);

    }
    if (esfera) {
        float x, y, z, a, b;
        float inc, incb, b0;
        float r = 15;
        int cont;

        inc = 1 / r;
        strcpy(name, DATADIR);
        strcat(name, "deathstar.dat");
        fp = fopen(name, "wt");

        x = y = z = 0;
        cont = 0;
        for (a = inc; a < PI / 2; a += inc, cont++) {
            incb = 1.0 / (r * sin(a));
            if (incb > 2 * PI)
                incb = 2 * PI;
            if (a == 0)
                incb = 2 * PI;
            b0 = cont % 2 ? 0 : incb / 2;
            for (b = b0; b < 2 * PI; b += incb) {
                x = r * sin(a) * cos(b);
                y = r * sin(a) * sin(b);
                z = r * cos(a);
                fprintf(fp, "%f %f %f %d\n", x, y, z, (z > 0) + 2);
                z = -r * cos(a);
                fprintf(fp, "%f %f %f %d\n", x, y, z, (z > 0) + 2);
            }
        }
        fprintf(fp, "\n");

        fclose(fp);
    }

    if (onda) {
        float x, y, z, t;
        float inc = .1;
        int i, j;

        strcpy(name, DATADIR);
        strcat(name, "onda.dat");
        fp = fopen(name, "wt");

        x = y = z = 0;
        inc = .5;
        for (t = 0; t < 10; t += .4) {
            for (i = 0; i < 50; i++) {
                for (j = 0; j < 25; j++) {
                    x = (float) i;
                    y = (float) j;
                    z = 1 * sin(x / 3. + t);
                    fprintf(fp, "%f %f %f %d\n", x, y, z, 1);
                }
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }

    if (unaD) {
        float x, t;
        float inc;

        strcpy(name, DATADIR);
        strcat(name, "unaD.dat");
        fp = fopen(name, "wt");

        x = 0;
        inc = .5;
        for (t = 0; t < 20; t += .1) {
            for (i = 0; i < 15; i++) {
                x = cos(t) + i + cos(2 * t);
                fprintf(fp, "%f %g\n", x, ((float) i + 2) / 20);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }

    if (lisa) {
        float x, y, t;
        float inc;

        strcpy(name, DATADIR);
        strcat(name, "lisa.dat");
        fp = fopen(name, "wt");

        x = 0;
        inc = .5;
        for (t = 0; t < 20; t += .1) {
            for (i = 0; i < 15; i++) {
                x = cos(t) + i + cos(2 * t);
                y = sin(t) + i;
                fprintf(fp, "%f %f %d\n", x, y, i + 2);
            }
            //      fprintf(fp,"\n");
        }
        fclose(fp);
    }


    if (fuente) {
        float x[N], y[N], z[N], vx[N], vy[N], vz[N], v;
        float radio[N];
        float x0, y0, z0;
        int paso = 0;
        float it = .1;
        float t = 0;
        float a, b, c;
        int num = 0;
        int nummax = 2000;

        x0 = y0 = z0 = 0;
        if (num > N)
            exit(EXIT_FAILURE);
        for (i = 0; i < num; i++) {
            vx[i] = vy[i] = vz[i] = x[i] = y[i] = z[i] = 0;
        }
        strcpy(name, DATADIR);
        strcat(name, "fuente.dat");
        fp = fopen(name, "wt");
        fprintf(fp, "#Time: %f paso: %d", t, paso);
        fprintf(fp, " x:%f y:%f z:%f r:%f v:%f\n",
                x0, y0, z0, radio[i], 0.0);

        fprintf(fp, "%f %f %f %f %d %f\n", -500., -500., -500., 5., 2,
                0.0);
        fprintf(fp, "%f %f %f %f %d %f\n\n", 500., 500., 500., 10., 2,
                0.0);

        for (paso = 1; paso < 200; paso++) {
            for (i = num; i < nummax; i++) {
                a = (float) (rand() % 1000);
                if (a < 10 || 1) {
                    a = (float) (rand() % 1000) / 1000. - .5;
                    b = (float) (rand() % 1000) / 1000. * 2 * 3.1416;
                    c = (float) (rand() % 1000) / 1000. * 3.1416;
                    vx[num] = a * sin(c) * cos(b);
                    vy[num] = a * sin(c) * sin(b);
                    vz[num] = a * cos(c);
                    radio[num] = (float) (rand() % 1000) / 1000. + .0001;
                    num++;
                }

            }
            fprintf(fp, "#Time: %f paso: %d", t, paso);
            fprintf(fp, " x:%E y:%E z:%g r:%g\n", x0, y0, z0, radio[i]);

            fprintf(fp, "%f %f %f %f %d %f\n", -1., -1., -1., 1., 2, 0.);
            fprintf(fp, "%f %f %f %f %d %f\n", 1., 1., 1., 1., 2, 0.);

            for (j = 0; j < num; j++) {
                x0 = x[j] + vx[j] * t;
                y0 = y[j] + vy[j] * t;
                z0 = z[j] + vz[j] * t;
                x[j] = x0;
                y[j] = y0;
                z[j] = z0;
                v = sqrt(vx[j] * vx[j] + vy[j] * vy[j] + vz[j] * vz[j]);
                fprintf(fp, "%f %f %f %f %d %f\n",
                        x0, y0, z0, radio[i] + t, j % 100, v);
            }

            //fprintf(fp,"#comment: time: %f\n       \t     \n",t);
            fprintf(fp, "\n");
            t += it;
        }

        fclose(fp);
    }
    if (fuente2) {
        float x[N], y[N], z[N], vx[N], vy[N], vz[N];
        float radio[N];
        float x0, y0, z0;
        int max = 1, paso;
        float it = .1;
        float t = 0;
        float a, b, c;
        float SIZE = 4;
        float VEL = .01;
        int num = 5000;

        if (num > N)
            exit(EXIT_FAILURE);
        for (i = 0; i < num; i++) {
            vx[i] = vy[i] = x[i] = y[i] = 0;
        }
        strcpy(name, DATADIR);
        strcat(name, "fuente.xyz");
        fp = fopen(name, "wt");

        fprintf(fp, "%d\n\n", 2);
        fprintf(fp, "Fe %f %f %f\n", -SIZE, -SIZE, -SIZE);
        fprintf(fp, "Fe %f %f %f\n", SIZE, SIZE, SIZE);

        for (i = 0; i < num; i++) {
            a = (float) (((rand() % 1000) / 1000. - .5) * VEL);
            b = (float) ((rand() % 1000) / 1000. * 2 * 3.1416);
            c = (float) ((rand() % 1000) / 1000. * 3.1416);
            vx[i] = a * sin(c) * cos(b);
            vy[i] = a * sin(c) * sin(b);
            vz[i] = a * cos(c);
            radio[i] = (float) (rand() % 1000) / 1000. + .0001;
            max++;
        }

        for (paso = 0; paso < 200; paso++) {
            fprintf(fp, "%d\n\n", 2 + paso);
            fprintf(fp, "Fe %f %f %f\n", -SIZE, -SIZE, -SIZE);
            fprintf(fp, "Fe %f %f %f\n", SIZE, SIZE, SIZE);

            for (j = 0; j < num; j++) {
                if (vx[j] != 0 || vy[j] != 0 || vz[j] != 0) {
                    x0 = x[j] + vx[j] * t;
                    y0 = y[j] + vy[j] * t;
                    z0 = z[j] + vz[j] * t;
                    x[j] = x0;
                    y[j] = y0;
                    z[j] = z0;
                    fprintf(fp, "Fe %f %f %f\n", x0, y0, z0);
                }
            }
            //    fprintf(fp,"#comment: time: %f\n",t);
            //fprintf(fp,"#comment: time: %f\n       \t     \n",t);
            //      fprintf(fp,"\n");
            t += it;
        }

        fclose(fp);
    }

    if (colors) {
        float x, y, z, x0;
        int cont = 0;

        strcpy(name, DATADIR);
        strcat(name, "colors.dat");
        fp = fopen(name, "wt");

        x = y = z = 0;
        x0 = 0;
        for (y = x0; y < 32; y += 1) {
            for (x = x0; x < 32; x += 1) {
                fprintf(fp, "%f %f %d\n", x, y, cont++);
            }
        }
        fprintf(fp, "\n");
        fclose(fp);
    }
    if (infinito) {
        int i, j;
        float r;
        float x, y, z;

        strcpy(name, DATADIR);
        strcat(name, "infinito.dat");
        fp = fopen(name, "wt");

        j = 0;
        for (i = 0; j < 100000; j++) {
            r = (float) ((float) rand() / RAND_MAX) / 10;
            x = (float) ((float) rand() / RAND_MAX) * 10 - 5;
            y = (float) ((float) rand() / RAND_MAX) * 10 - 5;
            z = (float) ((float) rand() / RAND_MAX) * 10 - 5;

            fprintf(fp, "%f %f %f %f %d\n", x, y, z, r, rand() % 4096);

            //      fprintf(fp,"\n");
        }
        fclose(fp);
    }


    if (all) {
        int i, j;
        float r;
        float x, y, z;

        strcpy(name, DATADIR);
        strcat(name, "all.dat");
        fp = fopen(name, "wt");

        j = 0;
        for (j = 0; j < 65; j++) {
            for (i = 0; i < 65; i++) {
                r = (float) (i + 1);    //(float)((float)rand()/RAND_MAX)/10;      
                x = (float) ((float) rand() / RAND_MAX) * 600 - 300;
                y = (float) ((float) rand() / RAND_MAX) * 600 - 300;
                z = (float) ((float) rand() / RAND_MAX) * 600 - 300;

                fprintf(fp, "%f %f %f %f %d\n", x, y, z, r, j);
            }
            //      fprintf(fp,"\n");
        }
        fclose(fp);
    }


    if (campo2D) {
        float x, y, x0, y0;
        float vx, vy;
        float inc = .1;
        int cont = 0;
        int color;

        strcpy(name, DATADIR);
        strcat(name, "field2d.dat");
        fp = fopen(name, "wt");

        x = y = 0;
        x0 = y0 = .5;
        cont++;
        if (cont % 2)
            x0 = inc / 2;
        else
            x0 = 0;
        for (y = x0; y < 1; y += inc) {
            for (x = x0; x < 1; x += inc) {
                vx = x - x0;
                vy = y - y0;
                color = rand() % 32;
                fprintf(fp, "%f %f %f %f %d\n", x, y, vx, vy, color);
            }
        }

        fprintf(fp, "\n");
        fclose(fp);

    }
    if (campo3D) {
        float x, y, z, x0, y0, z0;
        float vx, vy, vz;
        float inc = .1;
        int cont = 0;

        strcpy(name, DATADIR);
        strcat(name, "field3d.dat");
        fp = fopen(name, "wt");

        x = y = z = 0;
        x0 = y0 = z0 = .5;
        for (z = 0; z < 1.; z += inc) {
            cont++;
            if (cont % 2)
                x0 = inc / 2;
            else
                x0 = 0;
            for (y = x0; y < 1.; y += inc) {
                for (x = x0; x < 1; x += inc) {
                    vx = (x - x0) / 10;
                    vy = (y - y0) / 10;
                    vz = (z - z0) / 10;
                    vz = .1;
                    vx = vy = .1;
                    fprintf(fp, "%f %f %f %f %f %f\n", x, y, z, vx, vy,
                            vz);
                }
            }
        }
        fprintf(fp, "\n");
        fclose(fp);
    }
    if (stars) {
        float x, y, z, size, color;
        long n = 20000;
        float h = 1000;

        strcpy(name, DATADIR);
        strcat(name, "stars.dat");
        fp = fopen(name, "wt");
        fprintf(fp, "%f %f %f %f %f\n", 0., 0., 0., 1., 2.);
        for (i = 0; i < n; i++) {
            x = ((float) rand() / RAND_MAX * h);
            y = ((float) rand() / RAND_MAX * h);
            z = 1000 + ((float) rand() / RAND_MAX * h);
            size = (float) rand() / RAND_MAX * 300 / sqrt(z);
            color = 1 + 40. / sqrt(z);
            fprintf(fp, "%f %f %f %f %f\n", x, y, z, size, color);
        }
        fprintf(fp, "\n");
        fclose(fp);
    }

    if (infinito2) {
        int i, j;
        float r;
        float x, y, z;
        long NUM = 50000;
        float size = 20;

        strcpy(name, DATADIR);
        strcat(name, "infinito.xyz");
        fp = fopen(name, "wt");

        j = 0;
        fprintf(fp, "%ld\n\n", NUM);
        for (i = 0; j < NUM; j++) {
            r = (float) ((float) rand() / RAND_MAX) / 100;
            x = (float) ((float) rand() / RAND_MAX) * size - (size * .5);
            y = (float) ((float) rand() / RAND_MAX) * size - (size * .5);
            z = (float) ((float) rand() / RAND_MAX) * size - (size * .5);

            fprintf(fp, "He %f %f %f\n", x, y, z);

            //      fprintf(fp,"\n");
        }
        fclose(fp);
    }


    return (0);
}
